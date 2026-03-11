#include "dash_item.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "config/palette.h"
// collision
#include "component/physics/component_collider.h"
// controller
#include "object/game_object_list.h"
#include "object/dash/game/dash_controller.h"
#include "object/dash/game/camera_follow.h"

void DashItem::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	m_components.Add<ComponentRendererMesh>(m_comp_id_render);
}

void DashItem::Update()
{
	switch (m_state)
	{
	case DashItemState::HURT:
	{
		UpdateHurt();
		break;
	}
	case DashItemState::TRIGGERING:
	{
		UpdateTriggering();
		break;
	}
	case DashItemState::TRIGGERING_BACK:
	{
		UpdateTriggeringBack();
		break;
	}
	case DashItemState::DESTROYING:
	{
		UpdateDestroying();
		break;
	}
	}
}

void DashItem::InitializeConfig(const DashItemConfig& config)
{
	m_config = config;
	m_transform.SetRotationYOnly(Math::PI * 0.25f);
	m_status.hp = m_config.hp_max;
	InitializeVisual();
	InitializeCollision();
}

void DashItem::OnDamage(float damage)
{
	if (m_state != DashItemState::ACTIVE && m_state != DashItemState::HURT)
	{
		return;
	}
	m_status.hp -= damage;
	EnterHurt();
}

DashItemTriggerInfo DashItem::OnTrigger()
{
	assert(m_state == DashItemState::DEAD);
	DashItemTriggerInfo trigger_info{}; // TODO: different trigger info
	EnterTriggering();
	return trigger_info;
}

void DashItem::InitializeVisual()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cylinder_4x1");
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ g_palette.color_dash_item.x, g_palette.color_dash_item.y, g_palette.color_dash_item.z };
		material_default.emission_color = material_default.base_color;
		material_default.emission_intensity = 0.8f;
		const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
		material.SetTechnique(material_default, render_mask);
		Model model{ model_desc, material, &m_transform };
		model.GetTransform().SetPositionY(m_config.height * 0.5f);
		model.GetTransform().SetScale(Vector3{ m_config.radius, m_config.height, m_config.radius });
		m_id_mesh_active = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.shading_model = ShadingModel::UNLIT;
		material_default.base_color = Vector3{ g_palette.color_dash_item.x, g_palette.color_dash_item.y, g_palette.color_dash_item.z };
		const auto& model_desc_cube = GetPresetManager().GetModelDesc("model/primitive/cube_wireframe");
		const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
		material.SetTechnique(material_default, render_mask);
		material.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
		Model model{ model_desc_cube, material, &m_transform };
		model.GetTransform().SetPositionY(m_config.height * 0.5f);
		model.GetTransform().SetScale(
			Vector3{ m_config.radius * Math::SQRT_2, m_config.height, m_config.radius * Math::SQRT_2 }
			+ Vector3{ 1.0f, 1.0f, 1.0f } * 0.01f
		);
		model.GetTransform().SetRotationYOnly(Math::PI * 0.25f);
		m_id_mesh_border = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.cull_type = CullType::CULL_NONE;
		material_default.no_deferred = true; // disable deferred rendering for mask
		material.SetTechnique(material_default);
		TechniqueDescForwardMask mask_desc{};
		mask_desc.stencil_ref = 1;
		material.SetTechnique(mask_desc);
		Model model{ model_desc, material, &m_transform };
		model.GetTransform().SetPositionY(m_config.height * 0.5f);
		model.GetTransform().SetScale(Vector3{ m_config.radius, m_config.height, m_config.radius });
		model.SetActive(false);
		m_id_mesh_dead = comp_render_mesh.AddModel(model);
	}

}

void DashItem::InitializeCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& colliders = comp_collider.GetColliders();
	colliders.clear(); // reset

	// collision
	PolygonCylinder cylinder{};
	cylinder.height = m_config.height;
	cylinder.radius = m_config.radius;
	cylinder.sides = 4;
	TransformNode3D shape_transform{};
	shape_transform.SetParent(&m_transform);
	shape_transform.SetPositionY(m_config.height * 0.5f);
	Collider collider{ &m_transform, ColliderLayer::Type::DASH_ITEM };
	collider.AddShape(ShapeCollider{ cylinder }, shape_transform);
	collider.bounce = 0.1f;

	comp_collider.AddCollider(collider);
}

void DashItem::EnterActive()
{
	m_state = DashItemState::ACTIVE;
}

void DashItem::EnterHurt()
{
	// hurt effects
	auto camera = GetOwner().FindGameObject<CameraFollow>();
	camera->SetShake({ Vector3{ 0.2f, 0.2f, 0.2f }, m_config.hurt_duration * 0.5f });
	
	// state transition
	m_state = DashItemState::HURT;
	m_status.state_countdown.Initialize(m_config.hurt_duration);
}

void DashItem::UpdateHurt()
{
	// update visual
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_active = comp_render_mesh.GetModel(m_id_mesh_active);
	auto& model_border = comp_render_mesh.GetModel(m_id_mesh_border);
	auto& model_active_mat = model_active.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();
	auto& model_border_mat = model_border.GetMaterialDesc().GetTechnique<TechniqueDescDefault>();

	float hp_height = m_config.height * (m_status.hp / m_config.hp_max);
	model_active.GetTransform().SetScaleY(hp_height);
	model_active.GetTransform().SetPositionY(hp_height * 0.5f);

	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		model_active_mat.emission_intensity = 0.8f;
		model_border_mat.emission_intensity = 0.8f;
		if (m_status.hp <= 0.0f)
		{
			EnterDead();
		}
		else
		{
			EnterActive();
		}
	}
	float intensity = Math::Lerp(0.8f, 2.0f, 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f); // TODO Tween
	model_active_mat.emission_intensity = intensity;
	model_border_mat.emission_intensity = intensity;
}

void DashItem::EnterDead()
{
	m_state = DashItemState::DEAD;
	// disable collision
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	// update visual
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	// comp_render_mesh.SetActive(false);
	auto& model_active = comp_render_mesh.GetModel(m_id_mesh_active);
	model_active.SetActive(false);
	auto& model_dead = comp_render_mesh.GetModel(m_id_mesh_dead);
	model_dead.SetActive(true);
	// add to dash controller
	auto dash_controller = GetOwner().FindGameObject<DashController>();
	assert(dash_controller);
	dash_controller->AddDashItem(std::dynamic_pointer_cast<DashItem>(shared_from_this()));
	m_status.registered = true;
}

void DashItem::EnterTriggering()
{
	m_state = DashItemState::TRIGGERING;
	m_status.state_countdown.Initialize(m_config.trigger_duration);
}

void DashItem::UpdateTriggering()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_mask = comp_render_mesh.GetModel(m_id_mesh_dead);
	auto& model_border = comp_render_mesh.GetModel(m_id_mesh_border);

	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		model_mask.GetTransform().SetScale(m_config.trigger_scale_max);
		model_border.GetTransform().SetScale(m_config.trigger_scale_max);
		EnterTriggerDone();
	}
	float scale = Math::Lerp(1.0f, m_config.trigger_scale_max, 1.0f - t); // TODO Tween
	model_mask.GetTransform().SetScale(scale);
	model_border.GetTransform().SetScale(scale);
}

void DashItem::EnterTriggerDone()
{
	m_state = DashItemState::TRIGGER_DONE;
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_border = comp_render_mesh.GetModel(m_id_mesh_border);
	model_border.SetActive(false);
}

void DashItem::UpdateTriggeringBack()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_mask = comp_render_mesh.GetModel(m_id_mesh_dead);

	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		model_mask.GetTransform().SetScale(m_config.trigger_scale_max);
		EnterTriggerDone();
	}
	float scale = Math::Lerp(m_config.trigger_scale_max, 0.0f, 1.0f - t); // TODO Tween
	model_mask.GetTransform().SetScale(scale);
}

void DashItem::EnterDestroying()
{
	if (m_status.used)
	{
		return;
	}
	if (m_status.registered)
	{
		// remove from dash controller
		auto dash_controller = GetOwner().FindGameObject<DashController>();
		assert(dash_controller);
		dash_controller->RemoveDashItem(std::dynamic_pointer_cast<DashItem>(shared_from_this()));
		m_status.registered = false;
	}
	m_state = DashItemState::DESTROYING;
	m_status.state_countdown.Initialize(m_config.destroy_duration);
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
}

void DashItem::SetUsed()
{
	m_status.used = true;
}

void DashItem::UpdateDestroying()
{
	float t = m_status.state_countdown.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterDone();
		return;
	}
	// TODO
	// update visual
	m_transform.SetScale(t);
}

void DashItem::EnterTriggeringBack()
{
	m_state = DashItemState::TRIGGERING_BACK;
	m_status.state_countdown.Initialize(m_config.trigger_duration);
}


void DashItem::EnterDone()
{
	m_state = DashItemState::DONE;
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_render_mesh.SetActive(false);
	comp_collider.SetActive(false);
}