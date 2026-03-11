#pragma once

#include <array>
#include <unordered_map>
#include "render/render_path.h"
// passes
#include "render/pass/prepass/pass_depth.h"

class RenderPathDepth : public RenderPathBase
{
public:
	void Initialize() override;
	void Finalize() override {}
	void InitializeViewContext(RenderViewKey view_id, uint32_t width, uint32_t height) override;
	void UpdateViewContext(const RenderPathViewContext& view_context) override;
	void Draw(RenderViewKey view_key, const ViewContext& view_context) override;
	void GetEditorItems(RenderViewKey view_key, std::vector<EditorItem>& items) const override;
private:
	// textures
	struct InternalRenderTargets
	{
		RenderTarget pre_pass{};
	};
	//void BuildRenderTargets(uint32_t width, uint32_t height);
	// depth pre-pass
	PassDepth m_pass_depth{};
	InternalRenderTargets m_render_targets{};
};