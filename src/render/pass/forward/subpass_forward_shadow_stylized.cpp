#include "subpass_forward_shadow_stylized.h"

#include <DirectXMath.h>

#include "global_context.h"
#include "render/render_system.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/render_scene.h"
#include "render/render_camera.h"
#include "render/render_common.h"
#include "render/resource/buffer.h"

#include "shader_setting.h"
#include "render/dx_trace.h"

using namespace DirectX;

namespace
{
	struct CBData
	{
		XMFLOAT4X4 shadow_view_proj_mat;
		XMFLOAT3 shadow_light_dir;
		float _shadow_padding;
	};
}


void SubPassForwardShadowStylized::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	PassBaseGeometry::Initialize(device, context);
	m_ps = Shader::CreateShaderPixel(m_device, L"pixel_forward_shadow_stylized.cso");
	m_cb = Buffer::CreateConstantBuffer(m_device, sizeof(CBData));
}

void SubPassForwardShadowStylized::ResetRenderableIndices(CameraRenderLayer layer)
{
	PassBaseGeometry::ResetRenderableIndices(layer);
	auto shadow_cameras = GetRenderScene().GetCameraManager().GetActiveCamerasOfType(CameraType::SHADOW);
	if (shadow_cameras.empty())
	{
		m_shadow_camera = nullptr;
	}
	else
	{
		m_shadow_camera = shadow_cameras[0]; // TODO!! config
	}
}

bool SubPassForwardShadowStylized::ShouldRender(const MaterialDesc& material_desc, ModelType model_type) const
{
	return m_shadow_camera
		&& material_desc.IfTechnique(RenderTechnique::DEFAULT, m_curr_render_layer)
		&& material_desc.IfTechnique(RenderTechnique::FORWARD_SHADOW_STYLIZED, m_curr_render_layer);
}

void SubPassForwardShadowStylized::SetInfoPerModel(const ModelRenderInfo& model)
{
	PassBaseGeometry::SetInfoPerModel(model);
	SetInfoPerMaterial(model.key);
}

void SubPassForwardShadowStylized::SetInfoPerMaterial(const ModelRenderKey& key)
{
	// per object material
	const auto& material_resource = GetMaterialResourceManager();
	const auto& material = material_resource.GetMaterialDesc(key.material_id).GetTechnique<TechniqueDescDefault>(m_curr_render_layer);
	const auto& render_states = GetRenderStates();
	auto& texture_loader = GetTextureLoader();
	auto albedo = texture_loader.GetTexture(material.albedo_texture_id);
	m_context->PSSetShaderResources(0, 1, albedo.GetAddressOf());
	auto emission = texture_loader.GetTexture(material.emission_texture_id);
	m_context->PSSetShaderResources(1, 1, emission.GetAddressOf());
	SetCullState(material.cull_type);
	if (material.stencil_ref >= 0)
	{
		m_context->OMSetDepthStencilState(
			render_states.m_dss_depth_read_leq_stencil_read_eq.Get(), material.stencil_ref);
	}
	else
	{
		m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	}
}

void SubPassForwardShadowStylized::SetInfoPerDraw()
{
	PassBaseGeometry::SetInfoPerDraw(); // mvp matrix

	const auto& render_states = GetRenderStates();
	// ps
	m_context->PSSetShader(m_ps.Get(), nullptr, 0);
	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_wrap.GetAddressOf());
	m_context->PSSetSamplers(1, 1, render_states.m_ss_compare_less_linear_border.GetAddressOf()); // shadow

	// cbuffer
	if (m_shadow_camera)
	{
		CBData cb{};
		auto proj_mat = m_shadow_camera->GetProjectionMatrix(0);
		auto view_mat = m_shadow_camera->GetViewMatrix(0);
		auto view_proj_mat = view_mat * proj_mat;
		XMStoreFloat4x4(&cb.shadow_view_proj_mat, XMMatrixTranspose(view_proj_mat));
		XMStoreFloat3(&cb.shadow_light_dir, XMVector3Normalize(view_mat.r[2]));
		cb._shadow_padding = 0.0f;
		m_context->UpdateSubresource(m_cb.Get(), 0, nullptr, &cb, 0, 0);


		m_context->PSSetConstantBuffers(0, 1, m_cb.GetAddressOf());
		// shadow map
		auto texture_srv = m_shadow_camera->GetShaderResourceViewDepthStencil(0); // TODO: config
		m_context->PSSetShaderResources(5, 1, &texture_srv);
	}

	// om
	// m_context->OMSetDepthStencilState(render_states.m_dss_depth_enabled.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_alpha.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);
}
