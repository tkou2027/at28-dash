#include "enemy_attack_item.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "config/palette.h"
// collision
#include "component/physics/component_collider.h"
// references
#include "object/game_object_list.h"
#include "object/dash/game/room.h"

void EnemyAttackItem::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	m_components.Add<ComponentRendererMesh>(m_comp_id_render);
}

void EnemyAttackItem::InitializeConfig(const EnemyAttackItemConfig& config)
{
	m_config = config;
	m_transform.SetPosition(m_config.position);
	InitializeVisual();
	InitializeCollision();
	m_dead = false;
}

void EnemyAttackItem::EnterAttackPreExecute(float duration)
{
	if (m_dead)
	{
		return;
	}
	DoEnterPreExecuteCommon();
	DoEnterPreExecute();
	//if (!m_config.delay)
	//{
	//	DoEnterPreExecute();
	//}
}

void EnemyAttackItem::UpdateAttackPreExecute()
{
	if (m_dead)
	{
		return;
	}
	DoUpdatePreExecute();
	//if (!m_config.delay)
	//{
	//	DoUpdatePreExecute();
	//}
}

void EnemyAttackItem::EnterAttackExecute(float duration)
{
	if (m_dead)
	{
		return;
	}
	DoEnterExecute();
	m_delay_timer.Initialize(duration);
	//if (!m_config.delay)
	//{
	//	DoEnterExecute();
	//}
	//else
	//{
	//	DoEnterPreExecute();
	//}
}

void EnemyAttackItem::UpdateAttackExecute()
{
	if (m_dead)
	{
		return;
	}
	DoUpdateExecute();
	//if (!m_config.delay)
	//{
	//	DoUpdateExecute();
	//}
	//else
	//{
	//	DoUpdatePreExecute();
	//}
}

void EnemyAttackItem::EnterAttackCooldown(float duration)
{
	if (m_dead)
	{
		return;
	}
	DoEnterCooldown();
	//if (!m_config.delay)
	//{
	//	DoEnterCooldown();
	//}
	//else
	//{
	//	DoEnterExecute();
	//	//m_delay_timer.Initialize(3.0f); // TODO: config
	//}
}

void EnemyAttackItem::UpdateAttackCooldown()
{
	if (m_dead)
	{
		return;
	}
	DoUpdateExecute();
	//if (m_config.delay)
	//{
	//	DoUpdateExecute();
	//}
	//else
	//{
	//	DoUpdateExecute();
	//	//float t = m_delay_timer.Update(GetDeltaTime());
	//	//if (t <= 0.0f)
	//	//{
	//	//	if (!m_done_execute)
	//	//	{
	//	//		DoEnterExecute();
	//	//	}
	//	//	else
	//	//	{
	//	//		DoUpdateExecute();
	//	//	}
	//	//}
	//	//else
	//	//{
	//	//	DoUpdatePreExecute();
	//	//}
	//}
}

void EnemyAttackItem::ExitAttackCooldown()
{
	DoEnterCooldown();
	m_dead = true;
}

void EnemyAttackItem::DoEnterPreExecuteCommon()
{
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	auto& model_index = comp_render.GetModel(m_id_mesh_index);
	model_index.SetActive(true);
	auto& model_warn = comp_render.GetModel(m_id_mesh_warn);
	model_warn.SetActive(false);
	auto& model_attack = comp_render.GetModel(m_id_mesh_attack);
	model_attack.SetActive(false);

	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	auto& collider_attack = comp_collider.GetCollider(m_id_collider_attack);
	collider_attack.SetActive(false);
	auto& collider_index = comp_collider.GetCollider(m_id_collider_index);
	collider_index.SetActive(true);
}

void EnemyAttackItem::DoEnterPreExecute()
{
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	auto& model_warn = comp_render.GetModel(m_id_mesh_warn);
	model_warn.SetActive(true);
	auto& model_attack = comp_render.GetModel(m_id_mesh_attack);
	model_attack.SetActive(false);

	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	auto& collider_attack = comp_collider.GetCollider(m_id_collider_attack);
	collider_attack.SetActive(false);
	auto& collider_index = comp_collider.GetCollider(m_id_collider_index);
	collider_index.SetActive(true);

	m_animation_timer.Initialize(0.2f);
}

void EnemyAttackItem::DoUpdatePreExecute()
{
	float t = m_animation_timer.Update(GetDeltaTime());
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_warn = comp_render.GetModel(m_id_mesh_warn);
	if (t >= 0.5f)
	{
		model_warn.SetActive(true);
	}
	else
	{
		model_warn.SetActive(false);
	}
}

void EnemyAttackItem::DoEnterExecute()
{
	m_done_execute = true;
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(true);
	auto& collider_attack = comp_collider.GetCollider(m_id_collider_attack);
	collider_attack.SetActive(true);

	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render.SetActive(true);
	auto& model_warn = comp_render.GetModel(m_id_mesh_warn);
	model_warn.SetActive(false);
	auto& model_attack = comp_render.GetModel(m_id_mesh_attack);
	model_attack.SetActive(true);

	m_animation_timer.Initialize(0.03f);
}

void EnemyAttackItem::DoUpdateExecute()
{
	float t = m_animation_timer.Update(GetDeltaTime());
	auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model_attack = comp_render.GetModel(m_id_mesh_attack);
	if (t >= 0.5f)
	{
		model_attack.SetActive(true);
	}
	else
	{
		model_attack.SetActive(false);
	}
	//float t = m_delay_timer.Update(GetDeltaTime());
	//auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	//auto& model_attack = comp_render.GetModel(m_id_mesh_attack);
	//float scale_t = Math::Lerp(m_config.size, m_config.size_max, 1.0f - t);
	//model_attack.GetTransform().SetScale(scale_t);

	//auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	//auto& collider_attack = comp_collider.GetCollider(m_id_collider_attack);
	//collider_attack.GetTransformParent().SetScale(scale_t * Math::INV_SQRT_2);
}

void EnemyAttackItem::DoEnterCooldown()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	comp_collider.SetActive(false);
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render_mesh.SetActive(false);
}

void EnemyAttackItem::InitializeVisual()
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& models = comp_render_mesh.GetModels();
	models.clear(); // reset

	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_plane");
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/block_face.png");
		material_default.base_color = Vector3{ g_palette.color_enemy_shell.x, g_palette.color_enemy_shell.y, g_palette.color_enemy_shell.z };
		material_default.pre_pass = false;
		material.SetTechnique(material_default);


		Model model{ model_desc, material, &m_transform };
		model.GetTransform().SetScale(m_config.size);
		model.GetTransform().SetPositionY(0.1f);
		m_id_mesh_warn = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		// material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/block_border.png");
		material_default.base_color = Vector3{ g_palette.color_theme.x, g_palette.color_theme.y, g_palette.color_theme.z };
		material_default.pre_pass = false;
		// material_default.stencil_ref = 0;
		material.SetTechnique(material_default);

		Model model{ model_desc, material, &m_transform };
		model.GetTransform().SetScale(m_config.size);
		model.GetTransform().SetPositionY(0.1f);
		m_id_mesh_attack = comp_render_mesh.AddModel(model);
	}
	{
		MaterialDesc material{};
		TechniqueDescDefault material_default{};
		// material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/block_border.png");
		material_default.base_color = Vector3{ g_palette.color_enemy_shell.x, g_palette.color_enemy_shell.y, g_palette.color_enemy_shell.z };
		material_default.pre_pass = false;
		const auto render_mask = CameraRenderLayerMask::DEFAULT | CameraRenderLayerMask::CAST_SHADOW;
		material.SetTechnique(material_default, render_mask);
		const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
		Model model{ model_desc, material, &m_transform };
		model.GetTransform().SetScale(m_config.index_size);
		model.GetTransform().SetPositionY(m_config.index_pos_y);
		m_id_mesh_index = comp_render_mesh.AddModel(model);
	}

	comp_render_mesh.SetActive(false);
}

void EnemyAttackItem::InitializeCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);
	auto& colliders = comp_collider.GetColliders();
	colliders.clear(); // reset
	{
		// collision
		PolygonCylinder cylinder{};
		cylinder.height = m_config.size;
		cylinder.radius = m_config.size * Math::INV_SQRT_2;
		cylinder.sides = 4;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		shape_transform.SetPositionY(m_config.size * 0.5f);
		shape_transform.SetRotationYOnly(Math::PI * 0.25f);
		Collider collider{ &m_transform, ColliderLayer::Type::ENEMY_ATTACK };
		collider.AddShape(ShapeCollider{ cylinder }, shape_transform);

		collider.SetIsTrigger(true);
		m_id_collider_attack = comp_collider.AddCollider(collider);
	}

	{
		// collision
		PolygonCylinder cylinder{};
		cylinder.height = 5.0f;
		cylinder.radius = 5.0f * Math::INV_SQRT_2;
		cylinder.sides = 4;
		TransformNode3D shape_transform{};
		shape_transform.SetParent(&m_transform);
		shape_transform.SetPositionY(m_config.index_pos_y);
		shape_transform.SetScale(m_config.index_size);
		shape_transform.SetRotationYOnly (Math::PI * 0.25f);
		Collider collider{ &m_transform, ColliderLayer::Type::ENEMY };
		collider.AddShape(ShapeCollider{ cylinder }, shape_transform);

		collider.SetIsTrigger(true);
		m_id_collider_index = comp_collider.AddCollider(collider);
	}

	comp_collider.SetActive(false);
}