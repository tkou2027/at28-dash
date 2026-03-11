#include "game_manager.h"
#include "object/game_object_list.h"
#include "config/preset_manager.h"
#include "object/common/camera_shadow.h"
#include "object/dash/game/camera_follow.h"
#include "object/dash/game/player.h"
#include "object/dash/game/field.h"
#include "object/dash/common/game_skybox.h"
// system
#include "object/common/hit_stop_updater.h"
#include "object/common/window_controller.h"

#include "global_context.h"
#include "scene/scene_manager.h"

#include "config/audio_manager.h"
#include "platform/sound.h"

#include "shader_setting.h"

void GameManager::Initialize()
{
	// system
	GetOwner().CreateGameObject<HitStopUpdater>();
	GetOwner().CreateGameObject<WindowController>();
	// main camera
	auto camera = GetOwner().CreateGameObject<CameraFollow>();
	m_camera_main = camera;
	//// shadow camera
	auto camera_shadow = GetOwner().CreateGameObject<CameraShadow>();
	m_camera_shadow = camera_shadow;

	// skybox
	auto skybox = GetOwner().CreateGameObject<GameSkybox>();
	// player
	auto player = GetOwner().CreateGameObject<Player>();
	camera->SetTargetObject(player, { 0.0f, 20.0f, 00.0f });
	camera->SetFollowFixed({ 350.0f, 350.0f, -350.0f });

	camera_shadow->InitializeConfig(
		g_camera_presets.shadow_main, player,
		Vector3(1.0f, -3.0f, -1.0f), 100.0f, 1.0f, 400.0f);

	// field
	auto field = GetOwner().CreateGameObject<Field>();
	field->InitializeConfig(g_field_config);
	m_field = field;

	// bgm
	int bgm = g_global_context.m_audio_manager->LoadBgm(BgmName::BGM);
	g_global_context.m_sound->PlaySoundLoop(bgm, -1);
}

void GameManager::OnSceneStart()
{
	auto field = m_field.lock();
	field->StartFade();
	field->SetTransferActiveRoom(0);
}

void GameManager::Update()
{

	auto field = m_field.lock();
	if (field->IfDone())
	{
		g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_ENDING);
	}

	// editor updates ===
	auto camera = m_camera_main.lock();
	camera->SetCameraShapeConfig(camera->GetCameraShapeConfig());
	auto camera_shadow = m_camera_shadow.lock();
	//camera_shadow->SetLightDirection(g_shader_setting.light_direction);
}

void GameManager::GetEditorItem(std::vector<EditorItem>& items)
{
	// camera
	{
		auto& camera_shape_config = m_camera_main.lock()->GetCameraShapeConfig();

		EditorItem camera_item{};
		camera_item.label = "Game Camera";
		// fov
		{
			EditorProperty prop{};
			prop.label = "FOV";
			prop.data_ptr = &camera_shape_config.fov;
			prop.type = EditorPropertyType::FLOAT;
			prop.min = 0.1f;
			prop.max = 3.0f;
			camera_item.properties.push_back(prop);
		}
		// follow distance
		//{
		//	EditorProperty prop{};
		//	prop.label = "Follow CenterOffset";
		//	prop.data_ptr = &m_camera_main.lock()->GetTargetPositionOffset().x;
		//	prop.type = EditorPropertyType::FLOAT3;
		//	prop.min = -200.0f;
		//	prop.max = 200.0f;
		//	camera_item.properties.push_back(prop);
		//}
		// follow distance
		{
			EditorProperty prop{};
			prop.label = "Follow Offset";
			prop.data_ptr = &m_camera_main.lock()->GetFollowOffset().x;
			prop.type = EditorPropertyType::FLOAT3;
			prop.min = -500.0f;
			prop.max = 500.0f;
			camera_item.properties.push_back(prop);
		}

		items.push_back(camera_item);
	}
}
