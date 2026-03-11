#include "block_visual.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/render_resource.h"
int BlockVisual::GetOrCreateMaterialDescDefault() const
{
	auto& material_resource = GetMaterialResource();
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.base_color = Vector3{ 1.0f, 0.7f, 0.7f } * 0.1f;
	material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
	material_default.emission_intensity = 1.2f;
	material_default.stencil_ref = 0;
	const auto render_mask = CameraRenderLayerMask::DEFAULT;
	material_desc.SetTechnique(material_default, render_mask);
	material_desc.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
	material_desc.SetTechnique(TechniqueDescForwardShadowStylized{}, render_mask);
	int material_id = material_resource.AddMaterialDescIfNotExists("mat_block_default", material_desc);
	return material_id;
}

int BlockVisual::GetOrCreateMaterialDescMask() const
{
	auto& material_resource = GetMaterialResource();
	auto& texture_loader = GetTextureLoader();
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.albedo_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/tiles.png", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	material_default.stencil_ref = 1;
	const auto render_mask = CameraRenderLayerMask::DEFAULT;
	material_desc.SetTechnique(material_default, render_mask);
	material_desc.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
	int material_id = material_resource.AddMaterialDescIfNotExists("mat_block_mask", material_desc);
	return material_id;
}

void BlockVisual::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void BlockVisual::InitializeConfig()
{
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& preset_manager = GetPresetManager();
	// TODO: different block types
	{
		const ModelDesc& model_desc = preset_manager.GetModelDesc("model/floor_1");
		// const ModelDesc& model_desc = preset_manager.GetModelDesc("geo/unit_cube");
		int material_id = GetOrCreateMaterialDescDefault(); // batch material desc for blocks
		Model model{ model_desc, material_id, &m_transform };
		model.GetTransform().SetScale(1.01f);
		comp_mesh.AddModel(model);
	}
	{
		const ModelDesc& model_desc = preset_manager.GetModelDesc("geo/unit_plane");
		int material_id = GetOrCreateMaterialDescMask();
		Model model{ model_desc, material_id, &m_transform };
		comp_mesh.AddModel(model);
	}

}
