#include "player_bullets.h"
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/render_resource.h"
// physics
#include "physics/physics_scene.h"
// reference
#include "object/game_object_list.h"
#include "object/dash/game/time_controller.h"
#include "object/dash/game/dash_item.h"
#include "object/dash/game/enemy_prime.h"

// instance
void PlayerBulletInstance::Initialize(const BulletConfig& config, int index, PlayerBullets* owner)
{
	assert(owner);
	m_config = config;
	m_index = index;
	m_owner = owner;
	// status
	m_life = config.life;
	m_position = config.origin;

	auto& comp_render_mesh = m_owner->m_components.Get<ComponentRendererMesh>(m_owner->m_comp_id_render_mesh);
	auto& model = comp_render_mesh.GetModel(m_index);

	model.GetTransform().SetScale(Vector3{ 0.5f, 0.5f, 1.5f });
	float angle = atan2f(config.direction.x, config.direction.z);
	model.GetTransform().SetRotationYOnly(angle);
	model.SetActive(true);
	// TODO: rotation
	model.GetTransform().SetPosition(m_position);
}

void PlayerBulletInstance::Update()
{
	m_life -= m_owner->GetDeltaTime();
	auto& comp_render_mesh = m_owner->m_components.Get<ComponentRendererMesh>(m_owner->m_comp_id_render_mesh);
	auto& model = comp_render_mesh.GetModel(m_index);
	if (m_life <= 0.0f)
	{
		m_life = -1.0f;
		model.SetActive(false);
		return;
	}

	// move
	float move_distance = m_config.speed * m_owner->GetDeltaTime();
	// time stop effect
	const auto time_controller = m_owner->m_time_controller.lock();
	if (time_controller->IfInCountdown())
	{
		float max_distance = 10.0f; // TODO
		Vector3 movement = m_config.direction * move_distance;
		Vector3 target_pos = m_position + movement;
		target_pos = Vector3::MoveTowards(
			m_config.origin, target_pos, max_distance)
			+ movement * 0.2f; // move a little bit to avoid overlapping

		float distance_to_origin = (target_pos - m_config.origin).Length();
		float position_to_origin = (m_position - m_config.origin).Length();
		if (position_to_origin > distance_to_origin)
		{
			move_distance = 0.0f;
		}
		else
		{
			move_distance = (target_pos - m_position).Length();
		}
	}

	// collision
	Ray ray{ m_position, m_config.direction };
	auto& physics_scene = m_owner->GetPhysicsScene();
	CollisionInfo collision_info{};
	// TODO: robust?
	uint32_t layer_mask =
		ColliderLayer::MASK_BORDER| ColliderLayer::MASK_DASH_ITEM |
		ColliderLayer::MASK_ENEMY_SHELL | ColliderLayer::MASK_ENEMY_CORE;
	bool hit = physics_scene.CastRayXZ(
		ray, move_distance,
		layer_mask,
		collision_info
	);
	if (!hit)
	{
		m_position = m_position + m_config.direction * move_distance;
		model.GetTransform().SetPosition(m_position);
		return;
	}
	// update position to hit position
	m_position = collision_info.hit_info.hit_position;
	model.GetTransform().SetPosition(m_position);
	model.SetActive(false);
	m_life = -1.0f;
	// TODO: hit effect
	assert(collision_info.other);
	const auto& layer = collision_info.other->GetLayer();
	auto other_obj = collision_info.other->GetOwner();
	switch (layer)
	{
	case ColliderLayer::Type::BORDER:
	{
		// TODO plane ray cast does not work!!!!
		break;
	}
	case ColliderLayer::Type::ENEMY_CORE:
	{
		auto enemy_prime = std::dynamic_pointer_cast<EnemyPrime>(other_obj.lock());
		enemy_prime->OnDamaged(1.0f, collision_info.hit_info.hit_position); // TODO damage and hit position
		break;
	}
	case ColliderLayer::Type::DASH_ITEM:
	{
		auto dash_item = std::dynamic_pointer_cast<DashItem>(other_obj.lock());
		dash_item->OnDamage(1.0f); //TODO
		break;
	}
	}
}

void PlayerBulletInstance::Destroy()
{
	auto& comp_render_mesh = m_owner->m_components.Get<ComponentRendererMesh>(m_owner->m_comp_id_render_mesh);
	auto& model = comp_render_mesh.GetModel(m_index);
	model.SetActive(false);
	m_life = -1.0f;
}
// end instance

void PlayerBullets::Initialize()
{
	for (auto& bullet : m_bullets)
	{
		bullet.Reset();
	}
	// init render mesh
	auto& comp_render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render_mesh);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube"); // TODO

	// material
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.base_color = Vector3{ 1.0f,  1.0f,  0.5f };
	material_default.emission_color = Vector3{ 1.0f,  1.0f,  0.5f };
	material_default.emission_intensity = 1.8f;
	const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
	material_desc.SetTechnique(material_default, render_mask);
	// instancing
	auto& material_resource = GetMaterialResource();
	int material_id = material_resource.AddMaterialDescIfNotExists("mat_player_bullet", material_desc);
	// model
	Model model{ model_desc, material_id, &m_transform };
	model.SetActive(false);
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		comp_render_mesh.AddModel(model);
	}
}

void PlayerBullets::OnSceneInitialized()
{
	m_time_controller = GetOwner().FindGameObject<TimeController>();
	assert(!m_time_controller.expired());
}

void PlayerBullets::CreateBullet(const BulletConfig& config)
{
	for (int index = 0; index < MAX_BULLET; ++index)
	{
		auto& bullet = m_bullets[index];
		if (!bullet.IfUse())
		{
			bullet.Initialize(config, index, this);
			break;
		}
	}
}

void PlayerBullets::ClearBullets()
{
	for (auto& bullet : m_bullets)
	{
		if (bullet.IfUse())
		{
			bullet.Destroy();
		}
	}
}

void PlayerBullets::Update()
{
	for (auto& bullet : m_bullets)
	{
		if (bullet.IfUse())
		{
			bullet.Update();
		}
	}
}

