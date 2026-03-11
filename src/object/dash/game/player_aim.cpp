#include "player_aim.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "config/palette.h"
// other objects
#include "object/game_object_list.h"
#include "object/dash/game/dash_controller.h"
#include "object/dash/game/player.h"
#include "object/dash/game/dash_item.h"
#include "render/render_resource.h"

int PlayerAim::GetOrCreateMaterialDesc() const
{
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/aim_dot.png");
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.pre_pass = false;
	material_default.base_color = Vector3{g_palette.color_dash_item.x, g_palette.color_dash_item.y, g_palette.color_dash_item.z};
	// material_default.stencil_ref = 0;
	const auto render_mask = CameraRenderLayerMask::DEFAULT;
	material_desc.SetTechnique(material_default, render_mask);
	auto& material_resource = GetMaterialResource();
	int material_id = material_resource.AddMaterialDescIfNotExists("player_aim_default", material_desc);
	return material_id;
}

void PlayerAim::Initialize()
{
	 m_components.Add<ComponentRendererMesh>(m_comp_id_render_mesh);
}

void PlayerAim::OnSceneInitialized()
{
	m_dash_controller = GetOwner().FindGameObject<DashController>();
	assert(m_dash_controller.lock());
}

void PlayerAim::InitializeConfig(std::weak_ptr<Player> player)
{
	m_player = player;
	// init visual
	const auto& preset_manager = GetPresetManager();
	const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane");
	int material_id = GetOrCreateMaterialDesc();
	Model model{ model_desc, material_id, &m_transform };
	model.GetTransform().SetScaleX(m_config.line_width);
	model.GetTransform().SetPositionY(0.1f); // overlap with floor causes z-fighting, so lift it a bit
	model.SetActive(false);
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render_mesh);
	for (int i = 0; i < MAX_SEGMENTS; ++i)
	{
		comp_render_mesh.AddModel(model);
	}
}

void PlayerAim::Update()
{
	auto dash_controller = m_dash_controller.lock();
	auto player = m_player.lock();
	if (!dash_controller || !player)
	{
		return;
	}

	auto& dash_items = dash_controller->GetDashItems();
	int segment_index{ 0 };
	Vector3 from_center = player->GetTransform().GetPositionGlobal();
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render_mesh);
	for (; segment_index < dash_items.size() && segment_index < MAX_SEGMENTS; ++segment_index)
	{
		auto dash_item = dash_items[segment_index].lock();
		if (!dash_item)
		{
			continue;
		}
		Vector3 to_center = dash_item->GetTransform().GetPositionGlobal();
		Vector3 dir = to_center - from_center;
		float length = dir.Length() - m_config.gap; // TODO: radius
		if (length <= Math::EPSILON)
		{
			continue;
		}
		dir = dir.GetNormalized();
		
		auto& model = comp_render_mesh.GetModel(segment_index);
		model.GetTransform().SetPosition((from_center + to_center) * 0.5f + Vector3{0.0f, 0.1f, 0.0f});
		model.GetTransform().SetRotationYOnly(atan2f(dir.x, dir.z));
		model.GetTransform().SetScaleZ(length);
		model.GetUVAnimationState().uv_scroll_size.y = length / m_config.line_width;
		model.SetActive(true);

		from_center = to_center;
	}

	for (; segment_index < MAX_SEGMENTS; ++segment_index)
	{
		auto& model = comp_render_mesh.GetModel(segment_index);
		model.SetActive(false);
	}
}
