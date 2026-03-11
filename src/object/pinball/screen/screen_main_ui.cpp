#include "screen_main_ui.h"
#include "component/render/component_renderer_sprite.h"
#include "component/render/component_renderer_mesh.h"
#include "config/camera_names.h"
#include "config/preset_manager.h"
#include "config/palette.h"
#include "render/resource/texture_loader.h"

#include "object/game_object_list.h"
#include "object/pinball/camera_follow.h"
#include "component/render/component_camera.h"

using namespace DirectX;

void ScreenMainUI::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void ScreenMainUI::InitializeConfig(const EnemyCenterShapeConfig& shape_config)
{
	m_config.shape_height = shape_config.side_height;
	m_config.shape_radius = shape_config.side_radius;
	InitializeSprites();
	SetSpritesSize();
}

void ScreenMainUI::Update()
{
	auto camera_main = GetOwner().FindGameObject<CameraFollow>();
	const auto& camera_comp_id = camera_main->GetComponents().TryGetComponent<ComponentCamera>();
	const auto& comp_camera_main = GetOwner().GetComponentManager().GetComponent<ComponentCamera>(camera_comp_id);
	const Vector3 main_position = camera_main->GetTransform().GetPositionGlobal();
	const Vector3 main_target = comp_camera_main.GetTarget();
	const Vector3 main_up = comp_camera_main.GetUp();
	const auto& main_shape_config = comp_camera_main.GetShapeConfig();
	XMMATRIX mat_view = CameraMath::CalculateViewMatrix(main_position, main_target, main_up);
	XMMATRIX mat_proj = CameraMath::CalculateProjectionMatrix(main_shape_config);
	XMMATRIX mat_view_proj = mat_view * mat_proj;

	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& material_desc = comp_mesh.GetModel(1).GetMaterialDesc();
	auto& material_desc_projection = material_desc.GetTechnique<TechniqueDescForwardScreenProjection>(CameraRenderLayer::SCREEN_MAIN);
	XMStoreFloat4x4(&material_desc_projection.projector_mat_view_proj, XMMatrixTranspose(mat_view_proj));


	switch (m_state)
	{
	case State::IDLE:
	{
		break;
	}
	case State::ACTIVE:
	{
		UpdateAnimation();
		break;
	}
	case State::DELAY:
	{
		UpdateDelay();
		break;
	}
	}
}

void ScreenMainUI::UpdateAnimation()
{
}

void ScreenMainUI::UpdateDelay()
{

}

void ScreenMainUI::SetAimInfo(const AimInfo& aim_info)
{
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.center_u = aim_info.rotation_ratio;
	mat.center_v = aim_info.height_ratio;
	mat.radius = aim_info.radius;
	mat.thickness = aim_info.thickness;
}

void ScreenMainUI::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& texture_loader = GetTextureLoader();
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescForwardScreenBackground material_screen_bg{};
		material_screen_bg.color_inside = g_palette.color_player;
		// material_screen_bg.color_inside.w = 0.3f; // for mask
		material_screen_bg.color_stripe_positive = g_palette.color_theme;
		material_screen_bg.color_stripe_negative = g_palette.color_theme_accent;
		material_desc.SetTechnique(material_screen_bg, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		mesh_comp.AddModel(model);
	}
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane_xy");
		MaterialDesc material_desc{};
		TechniqueDescForwardScreenProjection material_screen_projection{};
		material_screen_projection.shape_height = m_config.shape_height;
		material_screen_projection.shape_radius = m_config.shape_radius;
		material_screen_projection.world_texture_id = texture_loader.GetOrCreateRenderTextureId(g_camera_presets.monitor.name);
		material_desc.SetTechnique(material_screen_projection, CameraRenderLayerMask::SCREEN_MAIN);
		Model model{ model_desc, material_desc, &m_transform };
		mesh_comp.AddModel(model);
	}
}

void ScreenMainUI::SetSpritesSize()
{
	auto& camera_preset = g_camera_presets.screen_main;
	const float screen_width = static_cast<float>(camera_preset.width);
	const float screen_height = static_cast<float>(camera_preset.height);
	const float center_x = screen_width * 0.5f;
	// mesh
	const float center_y = screen_height * 0.5f;
	auto& mesh_comp = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& model = mesh_comp.GetModel(0);
	model.GetTransform().SetScale({ screen_width, screen_height, 1.0f });
	model.GetTransform().SetPosition({ center_x, center_y, 0.5f }); // 0.5f to avoid clipping
	auto& mat = model.GetMaterialDesc().GetTechnique<TechniqueDescForwardScreenBackground>(CameraRenderLayer::SCREEN_MAIN);
	mat.radius = 0.6f;
	mat.center_u = 0.8f;
	mat.center_v = 0.05f;
}
