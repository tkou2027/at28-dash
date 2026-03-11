#include "subpass_forward_mask.h"

#include <DirectXMath.h>

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

void SubPassForwardMask::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
}

bool SubPassForwardMask::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_MASK, m_curr_render_layer);
}

void SubPassForwardMask::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardMask::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material = material_resource.GetMaterialDesc(key.material_id).GetTechnique<TechniqueDescForwardMask>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	m_context->OMSetDepthStencilState(GetRenderStates().m_dss_depth_read_stencil_write.Get(), material.stencil_ref);
}

void SubPassForwardMask::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix
	const auto& render_states = GetRenderStates();
	// called by base draw
	m_context->RSSetState(render_states.m_rs_cull_back.Get());
	m_context->PSSetShader(nullptr, nullptr, 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
