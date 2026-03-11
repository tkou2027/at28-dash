#include "lockon_ui.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/resource/texture_loader.h"
#include "render/render_resource.h"
#include "util/tween.h"

void LockOnUI::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_render);
}

void LockOnUI::InitializeConfig(const LockOnUiConfig& config)
{
	m_config = config;

	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	const auto& preset_manager = GetPresetManager();
	{
		const auto& model_desc = preset_manager.GetModelDesc("geo/unit_plane");
		int material_id = GetOrCreateMaterialDesc();
		Model model{ model_desc, material_id, &m_transform };
		model.GetTransform().SetScale(config.size_min);
		comp_render_mesh.AddModel(model);
	}
}

void LockOnUI::Update()
{
	switch (m_state)
	{
	case State::ACTIVE:
	{
		UpdateActive();
		break;
	}
	}
}

void LockOnUI::SetAnimationActive(bool active)
{
	if (active)
	{
		EnterActive();
	}
	else
	{
		EnterIdle();
	}
}

int LockOnUI::GetOrCreateMaterialDesc() const
{
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.albedo_texture_id = GetTextureLoader().GetOrLoadTextureFromFile("asset/texture/bumper_icon.png");
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.pre_pass = false;
	const auto render_mask = CameraRenderLayerMask::DEFAULT;
	material_desc.SetTechnique(material_default, render_mask);
	auto& material_resource = GetMaterialResource();
	int material_id = material_resource.AddMaterialDescIfNotExists("lock_on_ui_default", material_desc);
	return material_id;
}

void LockOnUI::SetVisible(bool visible)
{
	auto& comp_render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	comp_render_mesh.GetModel(0).SetActive(visible);
}

void LockOnUI::EnterIdle()
{
	m_state = State::IDLE;
}

void LockOnUI::EnterActive()
{
	m_state = State::ACTIVE;
	m_animation_timer.Initialize(m_config.animation_interval); // TODO: config
}

void LockOnUI::UpdateActive()
{
	float t = m_animation_timer.Update(GetDeltaTime());

}