#include "camera_names.h"
#include "config/constant.h"

#include "render/render_path.h"
#include "render/path/render_path_main.h"
#include "render/path/render_path_depth.h"

namespace
{
	CameraPreset g_preset_main
	{
		"main", // name
		SCREEN_WIDTH, // width
		SCREEN_HEIGHT, // height
		CameraRenderLayer::DEFAULT, // render_layer
		1 // render order
	};

	CameraPreset g_preset_shadow_main
	{
		"shadow_main", // name
		1024, // width
		1024, // height
		CameraRenderLayer::CAST_SHADOW, // render_layer
		0 // render order
	};
}

CameraPresets g_camera_presets{};

void CameraPathConfig::InitializeRenderPath(RenderPathManager& render_path_manager)
{
	// main
	g_camera_presets.main = g_preset_main;
	g_camera_presets.main.render_path_id = render_path_manager.AddPath(std::make_unique<RenderPathMain>());

	g_camera_presets.shadow_main = g_preset_shadow_main;
	g_camera_presets.shadow_main.render_path_id = render_path_manager.AddPath(std::make_unique<RenderPathDepth>());
}
