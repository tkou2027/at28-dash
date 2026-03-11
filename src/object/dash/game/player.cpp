#include "player.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "component/physics/component_collider.h"
// physics
#include "physics/physics_scene.h"
// other objects
#include "object/game_object_list.h"
#include "object/common/hit_stop_updater.h"
#include "object/dash/game/camera_follow.h"
#include "object/dash/game/field.h"
#include "object/dash/game/dash_controller.h"
#include "object/dash/game/time_controller.h"
#include "object/dash/game/enemy_attack_item.h"
// parts
#include "object/dash/game/player_bullets.h"
#include "object/dash/game/player_aim.h"
// player uitls
#include "object/dash/game/game_util/player_input.h"
#include "object/dash/game/game_util/player_animation_index.h"

#include "global_context.h"
#include "platform/controller.h"
#include "platform/keyboard.h"
#include "platform/sound.h"
#include "config/audio_manager.h"

#include "util/debug_ostream.h"

void Player::Initialize()
{
	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render);
	auto& comp_collider = m_components.Add<ComponentCollider>(m_comp_id_collider);
	// character model
	{
		const auto& model_desc = GetPresetManager().GetModelDesc("model/character");
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.metallic = 1.0f;
		material_default.roughness = 0.5f;
		//material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
		//material_default.emission_intensity = 2.0f;
		const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
		material.SetTechnique(material_default, render_mask);
		material.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
		material.SetTechnique(TechniqueDescForwardSilhouette{});
		Model model{ model_desc, material, &m_transform };
		// adjust loaded file
		model.GetTransform().SetScale(0.2f);
		model.GetTransform().SetPositionZ(0.0f);
		model.GetTransform().SetRotationYOnly(Math::PI);

		comp_render_mesh.AddModel(model);
	}
	// collision
	{
		Sphere sphere{};
		sphere.radius = 1.5f;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		shape_transform.SetPositionY(sphere.radius);
		Collider player_collider{ &m_transform, ColliderLayer::Type::PLAYER };
		player_collider.SetIsActor(true);
		player_collider.AddShape(ShapeCollider{ sphere }, shape_transform);
		comp_collider.AddCollider(player_collider);
	}
	// parts
	m_bullets = GetOwner().CreateGameObject<PlayerBullets>();

	m_aim = GetOwner().CreateGameObject<PlayerAim>();
	m_aim.lock()->InitializeConfig(std::dynamic_pointer_cast<Player>(shared_from_this()));
}

void Player::OnSceneInitialized()
{
	m_camera = GetOwner().FindGameObject<CameraFollow>();
	assert(m_camera.lock());
	m_hit_stop_updater = GetOwner().FindGameObject<HitStopUpdater>();
	assert(m_hit_stop_updater.lock());
	m_field = GetOwner().FindGameObject<Field>();
	assert(m_field.lock());
}

void Player::Update()
{
	// move state
	switch (m_state)
	{
	case MoveState::DEFAULT:
	{
		UpdateMove();
		UpdateShoot();
		break;
	}
	case MoveState::DASH:
	{
		UpdateDash();
		break;
	}
	case MoveState::CONTROLLED:
	{
		// position updated externally
		break;
	}
	}
	// hurt state
	switch (m_hurt_state)
	{
	case HurtState::HURT:
	{
		UpdateHurt();
		break;
	}
	}
}

bool Player::IfDead() const
{
	return m_hurt_state == HurtState::DEAD;
}

void Player::OnCollision(const CollisionInfo& collision)
{
	const Collider* other = collision.other;
	switch (other->GetLayer())
	{

	}
}

void Player::OnTrigger(const CollisionInfo& collision)
{
	const Collider* other = collision.other;
	switch (other->GetLayer())
	{
	case ColliderLayer::Type::ENEMY:
	{
		if (m_state == MoveState::DASH || m_field.lock()->GetTimeController().IfInCountdownOrRestore())
		{
			OnTriggerCancelOut(collision);
		}
	}
	case ColliderLayer::Type::ENEMY_ATTACK:
	{
		if (m_state == MoveState::DASH || m_field.lock()->GetTimeController().IfInCountdownOrRestore())
		{
			OnTriggerCancelOut(collision);
		}
		else
		{
			OnTriggerEnemyAttack(collision);
		}
		break;
	}
	}
}

void Player::ResetDefaultStates()
{
	// move state
	EnterMoveDefault();
	// hurt state
	m_hurt_state = HurtState::NONE;
	m_hp = m_status_config.max_hp;
	ResetHurtEffects();
}

void Player::EnterMoveDefault()
{
	m_state = MoveState::DEFAULT;
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.SetActive(true);
	collider.velocity = Vector3{};
	collider.gravity_scale = 1.0f;

	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model = comp_render.GetModel(0);
	auto& animation_state = model.GetAnimationState();
	animation_state.playing = true;
}

void Player::EnterControlled()
{
	// TODO: check if already controlled?
	m_state = MoveState::CONTROLLED;
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.velocity = Vector3{};
	collider.SetActive(false);
	// TODO: reset states?
	// position will be updated externally
	SetAnimationIndex(PlayerAnimationIndex::IDLE);
}

Vector3 Player::GetMoveInput() const
{
	Vector2 input = PlayerInput::GetInputMove();

	auto camera = m_camera.lock();
	Vector3 right{};
	Vector3 forward{};
	camera->GetDirectionXZNormalized(forward, right);
	Vector3 movement = right * input.x + forward * input.y;
	return movement;
}

void Player::UpdateMove()
{
	const Vector3 movement_input = GetMoveInput();
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);

	const auto& time_controller = m_field.lock()->GetTimeController();
	if (time_controller.IfInCountdownOrRestore())
	{
		collider.velocity = Vector3{}; // cannot move during countdown
		RotateToMoveDirection(movement_input);
	}
	else
	{
		m_desired_velocity = movement_input * m_move_config.max_speed_default;

		auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
		auto velocity = collider.velocity;

		float acc = m_move_config.max_acc_default;
		// acc = acc * (1.0f - m_speed_level / static_cast<float>(m_move_config.speed_level_max)); // TODO: balance
		velocity.x = Math::MoveTowards(velocity.x, m_desired_velocity.x, acc);
		velocity.z = Math::MoveTowards(velocity.z, m_desired_velocity.z, acc);
		collider.velocity = velocity;

		RotateToMoveDirection(movement_input);
		CheckEnterDash();
	}

	// animations
	if (movement_input.LengthSquared() > Math::EPSILON)
	{
		SetAnimationIndex(PlayerAnimationIndex::MOVE);
	}
	else
	{
		SetAnimationIndex(PlayerAnimationIndex::IDLE);
	}
}

void Player::UpdateShoot()
{
	// keyboard
	float t = m_shoot_cooldown_timer.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		if (PlayerInput::GetInputShoot())
		{
			auto& time_controller = m_field.lock()->GetTimeController();
			float cooldown = time_controller.IfInCountdownOrRestore() ? m_shoot_config.cooldown_time_stop : m_shoot_config.cooldown;
			m_shoot_cooldown_timer.Initialize(cooldown);
			auto bullets = m_bullets.lock();
			BulletConfig config{};
			config.speed = m_shoot_config.shoot_speed;
			config.direction = m_transform.GetForwardGlobal();
			config.origin = m_transform.GetPositionGlobal() + m_shoot_config.shoot_offset;
			bullets->CreateBullet(config);

			// shoot effects
			auto camera = m_camera.lock();
			CameraFollow::CameraShakeConfig shake_config{ {0.1f, 0.1f, 0.1f}, m_shoot_config.cooldown * 0.5f };
			camera->SetShake(shake_config);

			int se = g_global_context.m_audio_manager->LoadSoundEffect(SoundEffectName::SHOOT);
			g_global_context.m_sound->PlaySoundLoop(se, 0);
		}
	}
}

void Player::EnterHurt()
{
	m_hurt_state = HurtState::HURT;
	m_hurt_countdown.Initialize(m_status_config.hurt_duration);

	// hurt effects
	auto camera = m_camera.lock();
	CameraFollow::CameraShakeConfig shake_config{ {0.5f, 0.5f, 0.5f}, 0.2f };
	camera->SetShake(shake_config);

	// TODO: visual
	// TODO: SE

}

void Player::UpdateHurt()
{
	float t = m_hurt_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		if (m_hp <= 0.0f)
		{
			m_hurt_state = HurtState::DEAD;
		}
		else
		{
			m_hurt_state = HurtState::NONE;
		}
		ResetHurtEffects();
	}
}

void Player::ResetHurtEffects()
{
	// reset visuals
}

void Player::CheckEnterDash()
{
	if (PlayerInput::GetInputDash())
	{
		const auto& dash_controller = m_field.lock()->GetDashController();
		if (dash_controller.IfCanDash())
		{
			// enter dash
			m_dash_init_pos = m_transform.GetPosition();
			m_dash_target_pos = dash_controller.GetAndAssureDashTarget();

			Vector3 dash_dir = m_dash_target_pos - m_dash_init_pos;
			float dash_rotation = atan2f(dash_dir.x, dash_dir.z);
			m_transform.SetRotationYOnly(dash_rotation);

			auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
			auto& model = comp_render.GetModel(0);
			auto& animation_state = model.GetAnimationState();
			animation_state.playing = false;

			m_state = MoveState::DASH;
			m_state_countdown.Initialize(m_move_config.dash_duration);
		}
	}
}

void Player::UpdateDash()
{
	float t = m_state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		m_transform.SetPosition(m_dash_target_pos);

		auto camera = m_camera.lock();
		camera->SetShake({ Vector3{ 0.5f, 0.5f, 0.5f }, m_move_config.dash_duration });

		m_bullets.lock()->ClearBullets();

		auto& dash_controller = m_field.lock()->GetDashController();
		dash_controller.TriggerDashItem();
		EnterMoveDefault(); // TODO:
		return;
	}
	t = 1.0f - t;
	// m_transform.SetPosition(Vector3::Interpolate(m_dash_init_pos, m_dash_target_pos, t));
	Vector3 next_frame_target = Vector3::Interpolate(m_dash_init_pos, m_dash_target_pos, t);
	auto& collider = m_components.Get<ComponentCollider>(m_comp_id_collider).GetCollider(0);
	collider.velocity = next_frame_target - m_transform.GetPosition();
}

void Player::OnTriggerCancelOut(const CollisionInfo& collision)
{
	auto other_obj = collision.other->GetOwner();
	auto item = std::dynamic_pointer_cast<EnemyAttackItem>(other_obj.lock());
	if (item)
	{
		auto camera = m_camera.lock();
		CameraFollow::CameraShakeConfig shake_config{ {0.5f, 0.5f, 0.5f}, 0.2f };
		camera->SetShake(shake_config);
		item->ExitAttackCooldown();
	}
}

void Player::OnTriggerEnemyAttack(const CollisionInfo& collision)
{
	if (m_hurt_state != HurtState::NONE)
	{
		return; // already in hurt state, do not trigger again
	}
	m_hp = Math::Max(m_hp - 1.0f, 0.0f); // TODO: balance
	EnterHurt();
}

void Player::HitEffect(ColliderLayer::Type layer)
{
	int se = g_global_context.m_audio_manager->LoadSoundEffect(SoundEffectName::CLEAR);
	g_global_context.m_sound->PlaySoundLoop(se, 0);

	auto hit_stop_updater = m_hit_stop_updater.lock();
	const float hit_stop_duration = 0.1f;
	hit_stop_updater->SetHitStop(hit_stop_duration);
}

void Player::RotateToMoveDirection(const Vector3& move_dir)
{
	if (move_dir.LengthSquared() < Math::EPSILON)
	{
		return;
	}
	//Vector3 target_fwd = move_dir;
	//target_fwd.x = -target_fwd.x;
	//target_fwd.Normalize();
	//// quats
	//// auto q_current = m_transform.GetRotation().ToXMVECTOR();
	//auto up = DirectX::XMVectorSet(0, 1, 0, 0);
	//DirectX::XMVECTOR q_target = DirectX::XMQuaternionRotationMatrix(
	//	DirectX::XMMatrixLookToLH(DirectX::g_XMZero, target_fwd.ToXMVECTOR(), up)
	//);

	//Vector4 rotation_target{};
	//rotation_target.LoadXMVECTOR(q_target);
	//m_transform.SetRotation(rotation_target);



	//float inputRotation = atan2f(-move_dir.x, -move_dir.z);

	//if (inputRotation != m_transform.GetRotationY())
	//{
	//	float bufferY = m_transform.GetRotationY();
	//	bufferY = std::fmod(bufferY, Math::TWO_PI);

	//	if (bufferY > Math::PI)
	//	{
	//		bufferY -= Math::TWO_PI;
	//	}
	//	else if (bufferY < -Math::PI)
	//	{
	//		bufferY += Math::TWO_PI;
	//	}

	//	float rotationDiff = inputRotation - bufferY;
	//	if (rotationDiff > Math::PI)
	//		rotationDiff -= Math::TWO_PI;
	//	else if (rotationDiff < -Math::PI)
	//		rotationDiff += Math::TWO_PI;
	//	float rotationStep = 10 * GetDeltaTime();
	//	if (abs(rotationDiff) <= rotationStep)
	//		bufferY = inputRotation;
	//	else
	//		bufferY += (rotationDiff > 0.0f ? rotationStep : -rotationStep);

	//	m_transform.SetRotationYOnly(bufferY);
	//}


	float targetYaw = atan2f(move_dir.x, move_dir.z);


	DirectX::XMVECTOR q_current = m_transform.GetRotation().ToXMVECTOR();
	DirectX::XMVECTOR q_target = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0, 1, 0, 0), targetYaw);

	float t = 10.0f * GetDeltaTime();
	if (t > 1.0f) { t = 1.0f; }


	DirectX::XMVECTOR q_result = DirectX::XMQuaternionSlerp(q_current, q_target, t);

	Vector4 rotation_target{};
	rotation_target.LoadXMVECTOR(q_result);
	m_transform.SetRotation(rotation_target);
}

void Player::SetAnimationIndex(PlayerAnimationIndex index)
{
	// animations 
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model = comp_render.GetModel(0);
	auto& animation_state = model.GetAnimationState();
	animation_state.animation_index = static_cast<int>(index);
}
