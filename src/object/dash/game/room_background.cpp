#include "room_background.h"
#include "config/preset_manager.h"
#include "render/render_resource.h"
#include "component/render/component_renderer_mesh.h"

int RoomBackground::GetOrCreateMaterialDescDefault() const
{
	auto& material_resource = GetMaterialResource();
	MaterialDesc material_desc{};
	TechniqueDescDefault material_default{};
	material_default.shading_model = ShadingModel::UNLIT;
	material_default.base_color = Vector3{ 1.0f, 0.7f, 0.7f } * 0.1f;
	material_default.emission_color = Vector3{ 1.0f, 1.0f, 1.0f };
	material_default.emission_intensity = 2.0f;
	material_default.stencil_ref = 0;
	const auto render_mask = CameraRenderLayerMask::DEFAULT;
	material_desc.SetTechnique(material_default, render_mask);
	material_desc.SetTechnique(TechniqueDescDeferredCel{}, render_mask);
	int material_id = material_resource.AddMaterialDescIfNotExists("mat_background_default", material_desc);
	return material_id;
}

int RoomBackground::GetOrCreateMaterialDescMask() const
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
	int material_id = material_resource.AddMaterialDescIfNotExists("mat_background_mask", material_desc);
	return material_id;
}

void RoomBackground::Initialize()
{
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void RoomBackground::InitializeConfig(const std::string& key)
{
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	auto& preset_manager = GetPresetManager();
	auto& objects = preset_manager.GetCSVData(key);

	for (const auto& item : objects.items)
	{
		//if (item.name != "Floor1")
		//{
		//	continue;
		//}
		const ModelDesc& model_desc = preset_manager.GetModelDesc("model/" + item.name);
		int material_id = GetOrCreateMaterialDescDefault();
		Model model{ model_desc, material_id, &m_transform };
		model.GetTransform().SetScale(item.transform.m_scale);
		model.GetTransform().SetRotation(item.transform.m_rotation);
		model.GetTransform().SetPosition(item.transform.m_position);
		comp_mesh.AddModel(model);
	}
}
