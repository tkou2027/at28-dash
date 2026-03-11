#include "preset_manager.h"
#include "global_context.h"
#include "render/render_system.h"
#include "config/constant.h"
#include "config/palette.h"
#include "render/render_resource.h"
#include "util/resource_pool.h"

PresetManager::PresetManager() = default;
PresetManager::~PresetManager() = default;

void PresetManager::Initialize()
{
	m_model_pool = std::make_unique<ResourcePool<ModelDesc> >();
	m_sprite_pool = std::make_unique<ResourcePool<SpriteDesc> >();
	m_texture_pool = std::make_unique<ResourcePool<TextureResourceId> >();
	m_csv_pool = std::make_unique<ResourcePool<CSVData> >();
}

void PresetManager::LoadPresets()
{
	LoadSpritePresets();
	LoadMeshGeometryPresets();
	LoadModelPresets();
	LoadTexturePresets();
	LoadDataPresets();
}

void PresetManager::LoadSpritePresets()
{
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/text_press_enter.png";
		preset.size = Vector2{ 150.0f, 5.0f } *4.0f;
		RegisterAndLoadSpriteDesc("sprite/ui/button_enter", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/fade.png";
		preset.color = g_palette.color_theme;
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		preset.initial_uv_scroll_offset = { 128.0f, 1.0f };
		RegisterAndLoadSpriteDesc("sprite/ui/fade", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/title_bg.png";
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		RegisterAndLoadSpriteDesc("sprite/ui/bg_title", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ending_bg.png";
		preset.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		RegisterAndLoadSpriteDesc("sprite/ui/bg_ending", preset);
	}
	// status sprites ====
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_border.png";
		preset.color = g_palette.color_theme;
		// preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/border", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_bar_back.png";
		// preset.color = g_palette.color_theme;
		// preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/bar_back", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_bar_value.png";
		// preset.color = g_palette.color_theme;
		// preset.render_layer_mask = CameraRenderLayerMask::SCREEN_STATUS;
		RegisterAndLoadSpriteDesc("sprite/status/bar_value", preset);
	}
	// status sprites end ====
	// screen main sprites ====
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/status_border.png";
		// preset.color = g_palette.color_theme;
		// preset.render_layer_mask = CameraRenderLayerMask::SCREEN_MAIN;
		RegisterAndLoadSpriteDesc("sprite/main/border", preset);
	}
	{
		SpriteDesc preset{};
		preset.texture_path = "asset/texture/ui/testmask.png";
		// preset.color = g_palette.color_theme;
		// preset.render_layer_mask = CameraRenderLayerMask::SCREEN_MAIN;
		RegisterAndLoadSpriteDesc("sprite/main/testmask", preset);
	}
	// main sprites end ====
}

void PresetManager::LoadMeshGeometryPresets()
{
	auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	{
		ModelDesc preset{};
		preset.model_file = "unit_cube";
		MeshGeometry geometry{};
		Geometry::CreateBox(1.0f, 1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cube_face_uv";
		MeshGeometry geometry{};
		Geometry::CreateBox(1.0f, 1.0f, 1.0f, geometry);
		Geometry::SetBoxUV(geometry, true);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_plane";
		MeshGeometry geometry{};
		Geometry::CreatePlane(1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_plane_xy";
		MeshGeometry geometry{};
		Geometry::CreatePlaneXY(1.0f, 1.0f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_3x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinder(1.0f, 1.0f, 3, 1, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_3x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 3, 1, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinder(1.0f, 1.0f, 4, 1, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 4, 1, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_reversed_4x1";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 4, 1, true, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_6x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 6, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_32x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 32, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_cylinder_side_128x2";
		MeshGeometry geometry{};
		Geometry::CreateCylinderSide(1.0f, 1.0f, 128, 2, false, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "unit_square_ring_bevel";
		MeshGeometry geometry{};
		Geometry::CreateSquareRing(1.0f, 0.2f, 0.05f, geometry);
		model_loader.LoadMeshGeometry(preset, geometry);
		RegisterModelDesc("geo/" + preset.model_file, preset);
	}
}

void PresetManager::LoadModelPresets()
{
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/primitive/iso_sphere.fbx";
		RegisterAndLoadModelDesc("model/primitive/iso_sphere", preset);
	}
	{
		ModelDesc preset{};
		//preset.model_file = "asset/models/character.fbx";
		preset.model_file = "asset/models/primitive/cube_wireframe.fbx";
		RegisterAndLoadModelDesc("model/primitive/cube_wireframe", preset);
	}
	//{
	//	ModelDesc preset{};
	//	preset.model_file = "asset/models/vampire/dancing_vampire.dae";
	//	// preset.model_file = "asset/models/character/BaseCharacter.fbx";
	//	preset.has_animation = true;
	//	AnimationDesc animation_desc{};
	//	animation_desc.animation_file = "asset/models/vampire/dancing_vampire.dae";
	//	// animation_desc.animation_file = "asset/models/character/BaseCharacter.fbx";
	//	animation_desc.play_loop = true;
	//	preset.animations_desc.push_back(animation_desc);
	//	RegisterAndLoadModelDesc("model/test_animation", preset);
	//	// RegisterAndLoadModelDesc("model/character", preset);
	//}
	//{
	//	ModelDesc preset{};
	//	//preset.model_file = "asset/models/character.fbx";
	//	preset.model_file = "asset/models/character/chara_anim2.fbx";
	//	//preset.model_file = "asset/models/hew/block.fbx";
	//	//RegisterAndLoadModelDesc("model/character", preset);

	//	preset.has_animation = true;
	//	AnimationDesc animation_desc{};
	//	animation_desc.animation_file = "asset/models/character/chara_anim2.fbx";
	//	// animation_desc.animation_file = "asset/models/character/BaseCharacter.fbx";
	//	animation_desc.play_loop = true;
	//	preset.animations_desc.push_back(animation_desc);
	//	RegisterAndLoadModelDesc("model/character", preset);
	//}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/walking_embed.fbx";

		preset.has_animation = true;
		{
			AnimationDesc animation_desc{};
			animation_desc.animation_file = "asset/models/hew/standby animation3.fbx";
			animation_desc.play_loop = true;
			preset.animations_desc.push_back(animation_desc);
		}
		{
			AnimationDesc animation_desc{};
			animation_desc.animation_file = "asset/models/hew/walking_embed.fbx";
			animation_desc.play_loop = true;
			preset.animations_desc.push_back(animation_desc);
		}
		RegisterAndLoadModelDesc("model/character", preset);
	}
	//{
	//	ModelDesc preset{};
	//	preset.model_file = "asset/models/hew/girl_animation.fbx";

	//	preset.has_animation = true;
	//	AnimationDesc animation_desc{};
	//	animation_desc.animation_file = "asset/models/hew/girl_animation.fbx";
	//	animation_desc.play_loop = true;
	//	preset.animations_desc.push_back(animation_desc);
	//	RegisterAndLoadModelDesc("model/character", preset);
	//}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Bridge.fbx";
		RegisterAndLoadModelDesc("model/Bridge", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Gate.fbx";
		RegisterAndLoadModelDesc("model/Gate", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Wall1.fbx";
		RegisterAndLoadModelDesc("model/Wall1", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Wall2.fbx";
		RegisterAndLoadModelDesc("model/Wall2", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/floor_1.fbx";
		RegisterAndLoadModelDesc("model/Floor1", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Floor2.fbx";
		RegisterAndLoadModelDesc("model/Floor2", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Floor3.fbx";
		RegisterAndLoadModelDesc("model/Floor3", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/Lamppost.fbx";
		RegisterAndLoadModelDesc("model/Lamppost", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/WallBlock01.fbx";
		RegisterAndLoadModelDesc("model/WallBlock01", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/floor_1.fbx";
		RegisterAndLoadModelDesc("model/floor_1", preset);
	}
	{
		ModelDesc preset{};
		preset.model_file = "asset/models/hew/floor_1.fbx";
		RegisterAndLoadModelDesc("model/floor_1", preset);
	}


}

void PresetManager::LoadTexturePresets()
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	{
		const auto texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
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
		RegisterTexture("texture/skybox/mask", texture_id);
	}
	{
		const auto texture_id = texture_loader.GetOrLoadTextureCubeFromFile(
			"asset/texture/skybox/blink",
			{
				L"asset/texture/skybox/blink/right.png",
				L"asset/texture/skybox/blink/left.png",
				L"asset/texture/skybox/blink/top.png",
				L"asset/texture/skybox/blink/bottom.png",
				L"asset/texture/skybox/blink/front.png",
				L"asset/texture/skybox/blink/back.png"
			}
		);
		RegisterTexture("texture/skybox/default", texture_id);
	}
	{
		const auto texture_id = texture_loader.GetOrLoadTextureFromFile(
			"asset/texture/noise/3DNoise.png"
		);
		RegisterTexture("texture/noise/3DNoise", texture_id);
	}
}

void PresetManager::LoadDataPresets()
{
	RegisterCSVData("stage_01", "asset/data/stage_01.csv");
	RegisterCSVData("stage_02", "asset/data/stage_02.csv");
}

const ModelDesc& PresetManager::GetModelDesc(const std::string& key) const
{
	int id = m_model_pool->GetId(key);
	assert(id >= 0);
	return m_model_pool->Get(id);
}

const SpriteDesc& PresetManager::GetSpriteDesc(const std::string& key) const
{
	int id = m_sprite_pool->GetId(key);
	assert(id >= 0);
	return m_sprite_pool->Get(id);
}

const TextureResourceId& PresetManager::GetTextureId(const std::string& key) const
{
	int id = m_texture_pool->GetId(key);
	assert(id >= 0);
	return m_texture_pool->Get(id);
}

const CSVData& PresetManager::GetCSVData(const std::string& key) const
{
	int id = m_csv_pool->GetId(key);
	assert(id >= 0);
	return m_csv_pool->Get(id);
}

void PresetManager::RegisterModelDesc(const std::string& key, const ModelDesc& desc)
{
	m_model_pool->AddIfNotExists(key, desc);
}

void PresetManager::RegisterAndLoadModelDesc(const std::string& key, ModelDesc& desc)
{
	auto& model_loader = g_global_context.m_render_system->GetRenderResource().GetModelLoader();
	// load models
	model_loader.GetOrLoadModel(desc);
	// register
	m_model_pool->AddIfNotExists(key, desc);
}

void PresetManager::RegisterAndLoadSpriteDesc(const std::string& key, SpriteDesc& preset)
{
	auto& texture_loader = g_global_context.m_render_system->GetRenderResource().GetTextureLoader();
	preset.texture_id = texture_loader.GetOrLoadTextureFromFile(preset.texture_path, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	// split tile sheet
	for (auto& uv_desc : preset.uv_animation_desc)
	{
		uv_desc.uv_size_per_frame.x = uv_desc.uv_size.x / uv_desc.frame_cols;
		uv_desc.uv_size_per_frame.y = uv_desc.uv_size.y / uv_desc.frame_rows;
	}
	// register
	m_sprite_pool->AddIfNotExists(key, preset);
}

void PresetManager::RegisterTexture(const std::string& key, const TextureResourceId& texture_id)
{
	m_texture_pool->AddIfNotExists(key, texture_id);
}

void PresetManager::RegisterCSVData(const std::string& key, const std::string& filename)
{
	CSVData data{};
	CSVLoader::Load(filename, data);
	m_csv_pool->AddIfNotExists(key, data);
}
