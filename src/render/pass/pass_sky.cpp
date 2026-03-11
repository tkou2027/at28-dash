#include "pass_sky.h"

#include "render/render_common.h"
#include "render/render_scene.h"
#include "render/render_resource.h"
#include "render/render_states.h"
#include "render/resource/buffer.h"
#include "global_context.h"
#include "platform/timer.h"

#include "vertex_common.h"

namespace
{
	struct SkyConfig
	{
		float total_time;
		float _sky_padding_1;
		float _sky_padding_2;
		float _sky_padding_3;
	};
}

void PassSky::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	PassBase::Initialize(pDevice, pContext);
	// shaders
	m_vertex_shader = Shader::CreateShaderVertex(m_device, L"skybox_vertex.cso", Shader::InputLayoutType::MESH_STATIC);
	m_ps_fluid = Shader::CreateShaderPixel(m_device, L"fluidSkyTex_PS.cso");
	m_ps_static = Shader::CreateShaderPixel(m_device, L"skybox_pixel.cso");

	m_cb = Buffer::CreateConstantBuffer(m_device, sizeof(SkyConfig));
}

void PassSky::Draw()
{
	SetInfoPerDraw();
	// const auto& render_entities = GetRenderScene().m_objects_main;
	const auto& material_resource = GetMaterialResourceManager();
	// for (const auto& model : render_entities)
	const auto& renderable_manager = GetRenderScene().GetRenderablesManager();
	for (const auto& index : m_mesh_indices_static)
	{
		const auto& model = renderable_manager.m_models.at(index);
		const auto& material_desc = material_resource.GetMaterialDesc(model.key.material_id);
		if (!material_desc.IfTechnique(RenderTechnique::SKYBOX, m_camera_render_layer))// TODO
		{
			continue;
		}
		SetInfoPerModel(model);
		VertexCommon::DrawModelStatic(m_context, model, m_vertex_shader.layout_type);
	}
}

void PassSky::ResetRenderableIndices(CameraRenderLayer layer)
{
	m_mesh_indices_static.clear();
	m_camera_render_layer = layer;
}

void PassSky::AddRenderableIndex(size_t mesh_index, ModelType model_type, const MaterialDesc& material_desc)
{
	if (!material_desc.IfTechnique(RenderTechnique::SKYBOX, m_camera_render_layer) || model_type != ModelType::STATIC)
	{
		return;
	}
	m_mesh_indices_static.push_back(mesh_index);
}

void PassSky::SetInfoPerDraw()
{
	m_context->VSSetShader(m_vertex_shader.vertex_shader.Get(), nullptr, 0);
	//m_context->PSSetShader(m_ps_fluid.Get(), nullptr, 0);
	m_context->IASetInputLayout(m_vertex_shader.input_layout.Get());

	const auto& render_resource = GetRenderResource();
	m_context->VSSetConstantBuffers(0, 1, render_resource.m_buffer_per_projection.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, render_resource.m_buffer_per_view.GetAddressOf());

	const auto& render_states = GetRenderStates();
	m_context->RSSetState(render_states.m_rs_cull_front.Get());
	m_context->OMSetDepthStencilState(render_states.m_dss_depth_read.Get(), 0);
	m_context->OMSetBlendState(render_states.m_bs_disabled.Get(),
		render_states.m_blend_factor_zero, render_states.m_sample_mask_all);

	m_context->PSSetSamplers(0, 1, render_states.m_ss_linear_clamp.GetAddressOf());
	m_context->PSSetSamplers(5, 1, render_states.m_ss_linear_wrap.GetAddressOf());
}

void PassSky::SetInfoPerModel(const ModelRenderInfo& model)
{
	const auto& material_desc = GetMaterialResourceManager().GetMaterialDesc(model.key.material_id);
	const auto& material = material_desc.GetTechnique<TechniqueDescSkybox>(m_camera_render_layer);
	auto& texture_loader = GetTextureLoader();
	if (material.fluid)
	{
		m_context->PSSetShader(m_ps_fluid.Get(), nullptr, 0);
		auto noise_texture_id = texture_loader.GetOrLoadTextureFromFile("asset/texture/noise/3DNoise.png");
		auto noise_texture = texture_loader.GetTexture(noise_texture_id);
		m_context->PSSetShaderResources(0, 1, noise_texture.GetAddressOf());
		// constant buffer
		{
			SkyConfig cb{};
			cb.total_time = g_global_context.m_timer->GetTime();
			m_context->UpdateSubresource(m_cb.Get(), 0, nullptr, &cb, 0, 0);
		}
		m_context->PSSetConstantBuffers(0, 1, m_cb.GetAddressOf());
	}
	else
	{
		m_context->PSSetShader(m_ps_static.Get(), nullptr, 0);
		auto srv = texture_loader.GetTexture(material.skybox_texture_id);
		m_context->PSSetShaderResources(1, 1, srv.GetAddressOf());
	}

	const int stencil_ref = material.stencil_ref;
	const auto& render_states = GetRenderStates();
	if (stencil_ref >= 0)
	{
		m_context->OMSetDepthStencilState(render_states.m_dss_depth_read_leq_stencil_read_eq.Get(), stencil_ref);
	}
	else
	{
		m_context->OMSetDepthStencilState(render_states.m_dss_depth_read.Get(), 0);
	}
}
