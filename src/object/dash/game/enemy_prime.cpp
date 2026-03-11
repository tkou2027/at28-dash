#include "enemy_prime.h"
#include "component/physics/component_collider.h"
#include "object/game_object_list.h"
#include "object/dash/game/dash_item.h"
#include "object/dash/game/player.h"
#include "object/dash/game/room.h"
#include "object/dash/game/time_controller.h"
#include "object/common/hit_stop_updater.h"
#include "object/dash/game/enemy_prime_ui.h"
#include "object/dash/game/enemy_attack_item.h"
#include "object/dash/game/enemy_attack_phase_object.h"
#include "util/tween.h"

// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "config/palette.h"

void EnemyPrime::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	// render mesh
	m_components.Add<ComponentRendererMesh>(m_comp_id_render_mesh);
}

void EnemyPrime::InitializeConfig(const EnemyPrimeConfig& enemy_config, std::weak_ptr<Room> room)
{
	m_config = enemy_config;
	m_room = room;

	m_transform.SetParent(&room.lock()->GetTransform());
	m_transform.SetPosition(m_config.position);

	auto player = GetOwner().FindGameObject<Player>();
	assert(player);
	m_player = player;

	auto time_controller = GetOwner().FindGameObject<TimeController>();
	assert(time_controller);
	m_time_controller = time_controller;

	auto hit_stop_updater = GetOwner().FindGameObject<HitStopUpdater>();
	assert(hit_stop_updater);
	m_hit_stop_updater = hit_stop_updater;

	// attacks
	auto attack_pattern_aa = GetOwner().CreateGameObject<EnemyAttackPhasePatternAA>();
	m_attack_config = attack_pattern_aa->GetAttackConfig();
	m_attack_pattern_curr = attack_pattern_aa;

	InitializeCollision();
	InitializeVisual();
	InitializeStatus();
}

void EnemyPrime::Update()
{
	switch (m_state)
	{
	case EnemyState::IDLE:
	{
		break;
	}
	case EnemyState::TRANS_IN:
	{
		UpdateTransIn();
		break;
	}
	case EnemyState::ATTACK:
	{
		UpdateAttack();
		UpdateCollision();
		break;
	}
	case EnemyState::HURT:
	{
		UpdateHurt();
		UpdateCollision();
		break;
	}
	case EnemyState::EXIT:
	{
		UpdateExit();
		break;
	}
	}
}

void EnemyPrime::EnterTransIn(float duration)
{
	m_state = EnemyState::TRANS_IN;
	// timer
	m_status.state_countdown.Initialize(duration);
	// ui
	m_status_ui.lock()->EnterActive();
	// player
	auto player = m_player.lock();
	player->EnterControlled();

	m_transform.SetPositionY(m_config.position.y - 50.0f);
}

void EnemyPrime::UpdateTransIn()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		m_transform.SetPositionY(m_config.position.y);

		auto player = m_player.lock();
		player->EnterMoveDefault();
		EnterAttack();
		return;
	}
	// update visuals
	t = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_BACK, 1.0 - t);
	m_transform.SetPositionY(m_config.position.y - 50.0f + 50.0f * t);
}

void EnemyPrime::EnterAttack()
{
	//auto& attack_phases = m_config.attack_phases;
	m_state = EnemyState::ATTACK;
	//if (attack_phases.size() == 0 || attack_phases[0].attacks.size() == 0)
	//{
	//	m_status.attack_state = EnemyAttackState::IDLE;
	//	return;
	//}
	//m_status.attack_phase_index = 0;
	//m_status.attack_index = 0;
	if (!m_attack_pattern_curr.lock())
	{
		m_status.attack_state = EnemyAttackState::IDLE;
		return;
	}

	EnterAttackPrepare();
}

void EnemyPrime::OnDamaged(float damage, const Vector3& position)
{
	if (damage <= 0.0f)
	{
		return;
	}
	//if (m_state != EnemyState::ATTACK)
	//{
	//	return;
	//}
	// enter damaged state
	m_status.health = Math::Max(0.0f, m_status.health - damage);
	EnterHurt();
}

void EnemyPrime::RestoreCheckPointStates()
{
	if (m_attack_pattern_curr.lock())
	{
		m_attack_pattern_curr.lock()->RestoreCheckPointStates();
	}

	EnterAttackPrepare();
}

void EnemyPrime::InitializeCollision()
{
	// collision
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& colliders = comp_collider.GetColliders();
	colliders.clear(); // reset
	// shell
	{
		PolygonCylinder cylinder{};
		cylinder.height = m_config.size;
		cylinder.radius = m_config.size * Math::INV_SQRT_2;
		cylinder.sides = 4;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		// shape_transform.SetPositionY(m_config.size * 0.5f);
		shape_transform.SetRotationYOnly(Math::PI * 0.25f);
		Collider collider{ &m_transform, ColliderLayer::Type::ENEMY_SHELL };
		collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
		m_id_collider_shell = comp_collider.AddCollider(collider);
	}
	// core
	{
		PolygonCylinder cylinder{};
		cylinder.height = m_config.size;
		cylinder.radius = m_config.size * Math::INV_SQRT_2;
		cylinder.sides = 4;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		// shape_transform.SetPositionY(m_config.size * 0.5f * 0.5f);
		shape_transform.SetRotationYOnly(Math::PI * 0.25f);
		Collider collider{ &m_transform, ColliderLayer::Type::ENEMY_CORE };
		collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
		m_id_collider_core = comp_collider.AddCollider(collider);
	}

	// collision


	// core
}

void EnemyPrime::InitializeVisual()
{
	// model
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube_face_uv");
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ g_palette.color_enemy_shell.x, g_palette.color_enemy_shell.y, g_palette.color_enemy_shell.z };
		material_default.emission_color = material_default.base_color;
		material_default.emission_intensity = 0.8f;
		material_default.stencil_ref = 0;
		material.SetTechnique(material_default);
		Model model{ model_desc, material, &m_transform };
		// model.GetTransform().SetPositionY(m_config.size * 0.5f);
		model.GetTransform().SetScale(m_config.size);
		m_id_mesh_shell = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ 1.0f, 1.0f,  0.5f };
		material_default.stencil_ref = 1;
		material.SetTechnique(material_default);
		Model model{ model_desc, material, &m_transform };
		float core_size = m_config.size;
		// model.GetTransform().SetPositionY(core_size);
		model.GetTransform().SetScale(core_size);
		m_id_mesh_core = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ 0.01f, 0.07f, 0.07f };
		const auto& model_desc_cube = GetPresetManager().GetModelDesc("model/primitive/cube_wireframe");
		const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
		material.SetTechnique(material_default, render_mask);
		material.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
		Model model{ model_desc_cube, material, &m_transform };
		model.GetTransform().SetScale(m_config.size);
		m_id_mesh_border = comp_render_mesh.AddModel(model);
	}
}

void EnemyPrime::InitializeStatus()
{
	// initial health
	m_status.health = m_config.health;
	// status ui
	auto status_ui = GetOwner().CreateGameObject<EnemyPrimeUI>();
	status_ui->InitializeConfig(m_config);
	m_status_ui = status_ui;
}

void EnemyPrime::UpdateAttack()
{
	auto time_controller = m_time_controller.lock();
	if (time_controller->IfInCountdownOrRestore())
	{
		return;
	}

	switch (m_status.attack_state)
	{
	case EnemyAttackState::IDLE:
	{
		break;
	}
	case EnemyAttackState::PREPARE:
	{
		UpdateAttackPrepare();
		break;
	}
	case EnemyAttackState::FOLLOW:
	{
		UpdateAttackFollow();
		break;
	}
	case EnemyAttackState::PRE_EXECUTE:
	{
		UpdateAttackPreExecute();
		break;
	}
	case EnemyAttackState::EXECUTE:
	{
		UpdateAttackExecute();
		break;
	}
	case EnemyAttackState::COOLDOWN:
	{
		UpdateAttackCooldown();
		break;
	}
	}
	// TODO
	CheckAttackPhaseTransition();
}

void EnemyPrime::EnterAttackPrepare()
{
	m_state = EnemyState::ATTACK;
	//if (m_status.attack_phase_index < 0 || m_status.attack_index < 0)
	//{
	//	m_status.attack_state = EnemyAttackState::IDLE;
	//	return;
	//}
	m_status.attack_state = EnemyAttackState::PREPARE;
	//auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	//auto& attack_config = phase_config.attacks[m_status.attack_index];
	m_attack_pattern_curr.lock()->UpdatePatternIndex();
	m_attack_config = m_attack_pattern_curr.lock()->GetAttackConfig();
	
	// initialize timers
	m_status.attack_countdown.Initialize(m_attack_config.prepare_duration);
	m_attack_pattern_curr.lock()->EnterAttackPrepare(m_attack_config.prepare_duration);
	//for (auto& attack_item : m_attack_objects)
	//{
	//	attack_item.lock()->EnterAttackPrepare(m_attack_config.prepare_duration);
	//}
}

void EnemyPrime::UpdateAttackPrepare()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackFollow();
		return;
	}
	// TODO: update visuals
	m_attack_pattern_curr.lock()->EnterAttackPrepare(m_attack_config.prepare_duration);
	//for (auto& attack_item : m_attack_objects)
	//{
	//	attack_item.lock()->UpdateAttackPrepare();
	//}
}

void EnemyPrime::EnterAttackFollow()
{
	m_status.attack_state = EnemyAttackState::FOLLOW;
	// initialize timers
	m_status.attack_countdown.Initialize(m_attack_config.follow_duration);
	m_attack_pattern_curr.lock()->EnterAttackFollow(m_attack_config.follow_duration);
}

void EnemyPrime::UpdateAttackFollow()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackPreExecute();
		return;
	}
	m_attack_pattern_curr.lock()->UpdateAttackFollow();
}

void EnemyPrime::EnterAttackPreExecute()
{
	m_status.attack_state = EnemyAttackState::PRE_EXECUTE;
	// initialize timers
	m_status.attack_countdown.Initialize(m_attack_config.pre_execute_duration);
	// initialize attacks
	m_attack_pattern_curr.lock()->EnterAttackPreExecute(m_attack_config.pre_execute_duration);
}

void EnemyPrime::UpdateAttackPreExecute()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackExecute();
		return;
	}
	m_attack_pattern_curr.lock()->UpdateAttackPreExecute();
}

void EnemyPrime::EnterAttackExecute()
{
	m_status.attack_state = EnemyAttackState::EXECUTE;
	// initialize timers
	m_status.attack_countdown.Initialize(m_attack_config.execute_duration);
	// initialize attacks
	m_attack_pattern_curr.lock()->EnterAttackExecute(m_attack_config.execute_duration);
}

void EnemyPrime::UpdateAttackExecute()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterAttackCooldown();
		return;
	}
	m_attack_pattern_curr.lock()->UpdateAttackExecute();
}

void EnemyPrime::EnterAttackCooldown()
{
	m_status.attack_state = EnemyAttackState::COOLDOWN;
	// initialize timers
	m_status.attack_countdown.Initialize(m_attack_config.cooldown_duration);
	// initialize attacks
	m_attack_pattern_curr.lock()->EnterAttackCooldown(m_attack_config.cooldown_duration);
}

void EnemyPrime::UpdateAttackCooldown()
{
	float t = m_status.attack_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		m_attack_pattern_curr.lock()->ExitAttackCooldown();
		UpdateNextAttack();
		EnterAttack();
		return;
	}
	m_attack_pattern_curr.lock()->UpdateAttackCooldown();
}

void EnemyPrime::ExitAttackCooldown()
{
	DestroyItems();
}

void EnemyPrime::UpdateNextAttack()
{
	//auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
	//m_status.attack_index = (m_status.attack_index + 1) % phase_config.attacks.size();
	// m_attack_pattern_curr.lock()->UpdatePatternIndex();
}

void EnemyPrime::EnterHurt()
{
	m_state = EnemyState::HURT;
	m_status.state_countdown.Initialize(m_config.damage_delay);
	// ui
	auto status_ui = m_status_ui.lock();
	status_ui->SetValue(m_status.health);

	// hurt effects
	auto& model_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render_mesh);
	auto& model_core = model_render_mesh.GetModel(m_id_mesh_core);
	auto& model_core_mat = model_core.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();
	model_core_mat.base_color = Vector3{ g_palette.color_enemy_hurt.x, g_palette.color_enemy_hurt.y, g_palette.color_enemy_hurt.z };

	auto hit_stop_updater = m_hit_stop_updater.lock();
	hit_stop_updater->SetHitStop(0.1f);
}

void EnemyPrime::UpdateHurt()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	auto& model_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render_mesh);
	auto& model_core = model_render_mesh.GetModel(m_id_mesh_core);
	auto& model_core_mat = model_core.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();
	// clear all bumpers
	if (t <= 0.0f)
	{
		// return to attack state
		DestroyItems();
		model_core_mat.base_color = Vector3{g_palette.color_enemy_core.x, g_palette.color_enemy_core.y, g_palette.color_enemy_core.z};
		if (m_status.health <= 0)
		{
			// handle death
			EnterExit();
		}
		else
		{
			EnterAttack();
		}
		return;
	}
	if (t < 0.5)
	{
		t = 1.0f - t;
	}
	float t_lerp = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_QUAD, (1.0f - t) * 2.0f);
	// update visuals
	model_core_mat.base_color = Vector3{ g_palette.color_enemy_hurt.x, g_palette.color_enemy_hurt.y, g_palette.color_enemy_hurt.z };
}

void EnemyPrime::UpdateCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& shell_collider = comp_collider.GetCollider(m_id_collider_shell);
	auto time_controller = m_time_controller.lock();
	if (time_controller->IfInCountdownOrRestore())
	{
		shell_collider.SetActive(false);
	}
	else
	{
		shell_collider.SetActive(true);
	}
}

//void EnemyPrime::CreateAttackItems()
//{
//	m_attack_items.clear();
//	auto& phase_config = m_config.attack_phases[m_status.attack_phase_index];
//	auto& attack_config = phase_config.attacks[m_status.attack_index];
//	for (const auto& attack_item_config : attack_config.attack_items)
//	{
//		auto item = GetOwner().CreateGameObject<EnemyAttackItem>();
//		item->InitializeConfig(attack_item_config, m_room);
//		m_attack_items.push_back(item);
//	}
//}

void EnemyPrime::DestroyItems()
{
	for (auto& item_weak : m_active_items)
	{
		if (auto item = item_weak.lock())
		{
			item->EnterDestroying();
		}
	}
	m_active_items.clear();
}

void EnemyPrime::EnterExit()
{
	m_state = EnemyState::EXIT;
	// timer
	m_status.state_countdown.Initialize(m_config.exit_duration);
	// visuals
}

void EnemyPrime::UpdateExit()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		m_state = EnemyState::DONE;
	}
	// update visuals
		// update visuals
	// t = Tween::EaseFunc(Tween::TweenFunction::EASE_OUT_BACK, 1.0 - t);
	m_transform.SetPositionY(m_config.position.y - 50.0f * (1.0f - t) * (1.0f - t));
}
