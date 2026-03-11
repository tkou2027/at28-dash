#include "skybox.h"
#include "config/preset_manager.h"
#include "component/render/component_renderer_mesh.h"

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"

void Skybox::Initialize()
{
	auto& render_mesh = m_components.Add<ComponentRendererMesh>(m_comp_id_render);
	const auto& model_desc = GetPresetManager().GetModelDesc("geo/unit_cube");
	MaterialDesc mat_desc{};
	TechniqueDescSkybox mat_skybox{};
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	mat_skybox.skybox_texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
		"asset/texture/skybox/ablaze",
		{
			L"asset/texture/skybox/ablaze/right.png",
			L"asset/texture/skybox/ablaze/left.png",
			L"asset/texture/skybox/ablaze/top.png",
			L"asset/texture/skybox/ablaze/bottom.png",
			L"asset/texture/skybox/ablaze/front.png",
			L"asset/texture/skybox/ablaze/back.png"
		}
	);
	mat_desc.SetTechnique(mat_skybox);
	Model model{ model_desc ,mat_desc, nullptr };
	render_mesh.AddModel(model);
}

void Skybox::SetTexture(TextureResourceId texture_id)
{
	auto& render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model = render_mesh.GetModel(0);
	auto& material = model.GetMaterialDesc().GetTechnique<TechniqueDescSkybox>();
	material.skybox_texture_id = texture_id;
}

void Skybox::SetStencilRef(int stencil_ref)
{
	auto& render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model = render_mesh.GetModel(0);
	auto& material = model.GetMaterialDesc().GetTechnique<TechniqueDescSkybox>();
	material.stencil_ref = stencil_ref;
}

void Skybox::SetFluid(bool fluid)
{
	auto& render_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	auto& model = render_mesh.GetModel(0);
	auto& material = model.GetMaterialDesc().GetTechnique<TechniqueDescSkybox>();
	material.fluid = fluid;
}
