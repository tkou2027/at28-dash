#include "render/path/render_path_depth.h"
#include "global_context.h"
#include "render/render_system.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include <chrono>
#include "util/debug_ostream.h"
#include "render/pass/geometry/subpass_geometry_cel.h"
#include "render/pass/forward/subpass_forward_mask.h"
#include "render/pass/forward/subpass_forward_silhouette.h"
#include "render/pass/pass_reflection.h"

void RenderPathDepth::Initialize()
{
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* context = GetDeviceContext();

	// initialize passes
	// depth pre-pass
	m_pass_depth.Initialize(device, context);
}

void RenderPathDepth::InitializeViewContext(RenderViewKey view_key, uint32_t width, uint32_t height) {}

void RenderPathDepth::UpdateViewContext(const RenderPathViewContext& view_context)
{
	m_view_context = view_context;
	auto& scene_renderables = g_global_context.m_render_system->GetRenderScene().GetRenderablesManager();
	const auto& material_resource = g_global_context.m_render_system->GetRenderResource().GetMaterialManager();
	const auto render_layer = m_view_context.render_layer;
	const auto& visible_info = scene_renderables.GetRenderablesOfLayer(render_layer);

	m_pass_depth.ResetRenderableIndices(render_layer);
	// models
	for (const auto& index : visible_info.indices_model)
	{
		const auto& model_info = scene_renderables.m_models[index];
		const auto& material = material_resource.GetMaterialDesc(model_info.key.material_id);
		m_pass_depth.AddRenderableIndex(index, model_info.key.model_type, material);
	}
}

void RenderPathDepth::Draw(RenderViewKey view_key, const ViewContext& view_context)
{
	ID3D11DeviceContext* context = GetDeviceContext();
	// out
	view_context.render_target_out.ClearDepthStencil(context, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	view_context.render_target_out.Bind(context);

	m_pass_depth.Draw();

	ID3D11ShaderResourceView* nullSRVs[16] = { nullptr };
	context->VSSetShaderResources(0, 16, nullSRVs);
	context->PSSetShaderResources(0, 16, nullSRVs);
}

void RenderPathDepth::GetEditorItems(RenderViewKey view_key, std::vector<EditorItem>& items) const
{
	// TODO...
}
