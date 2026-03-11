#include "room.h"
// parts
#include "object/game_object_list.h"
// references
#include "object/dash/game/field.h"
#include "object/dash/game/player.h"
#include "object/dash/game/camera_follow.h"
#include "object/dash/game/dash_controller.h"
#include "object/dash/game/time_controller.h"
// parts
#include "object/dash/game/room_floor.h"
#include "object/dash/game/room_dash_items.h"
#include "object/dash/game/room_background.h"
#include "object/dash/game/enemy_prime.h"

void Room::Initialize()
{
}

void Room::OnSceneInitialized()
{
	m_player = GetOwner().FindGameObject<Player>();
	assert(m_player.lock());
	m_camera = GetOwner().FindGameObject<CameraFollow>();
	assert(m_camera.lock());
}

void Room::InitializeConfig(const FieldConfig& field_config, int id, std::weak_ptr<Field> field)
{
	m_field_config = field_config;
	m_room_id = id;
	m_config = field_config.rooms[id];
	m_field = field;

	InitializeFloor();
	InitializeDashItems();
	InitializeBackground();

	EnterRoomStateIdle();
}

void Room::Update()
{
	switch (m_state)
	{
	case RoomState::TRANS_IN_STAGE_1:
	{
		UpdateRoomStateTransInFirst();
		break;
	}
	case RoomState::TRANS_IN_STAGE_2:
	{
		UpdateRoomStateTransInSecond();
		break;
	}
	case RoomState::ACTIVE:
	{
		UpdateRoomStateActive();
		break;
	}
	case RoomState::TRANS_OUT_STAGE_1:
	{
		UpdateRoomStateTransOutFirst();
		break;
	}
	case RoomState::TRANS_OUT_STAGE_2:
	{
		UpdateRoomStateTransOutSecond();
		break;
	}
	}

	//auto& border_model = m_components.Get<ComponentRendererMesh>(m_comp_id_render).GetModel(border_index);
	//auto& uv_state = border_model.GetUVAnimationState();
	//uv_state.uv_scroll_offset = uv_state.uv_scroll_offset + Vector2{ 0.001f, 0.0f };
}

// states
void Room::EnterRoomStateIdle()
{
	m_state = RoomState::IDLE;
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->EnterRoomStateIdle();
	}
}

void Room::EnterRoomStateTransIn()
{
	m_state = RoomState::TRANS_IN_STAGE_1;
	const float duration{ m_action_config.trans_in_out_duration_stage_1 };
	m_state_countdown.Initialize(duration);
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->EnterRoomStateTransInFirst(duration);
	}
	auto player = m_player.lock();
	player->EnterControlled();
}

void Room::RestoreCheckPointStates()
{
	// player
	auto player = m_player.lock();
	player->ResetDefaultStates();

	// controllers
	auto field = m_field.lock();
	auto& dash_controller = field->GetDashController();
	dash_controller.Reset();
	auto& time_controller = field->GetTimeController();
	time_controller.Reset();

	// enemies
	for (auto& enemy : m_enemies)
	{
		enemy.lock()->RestoreCheckPointStates();
	}
}

void Room::UpdateRoomStateTransInFirst()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateTransInSecond();
		return;
	}
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->UpdateRoomStateTransInFirst();
	}
}

void Room::EnterRoomStateTransInSecond()
{
	m_state = RoomState::TRANS_IN_STAGE_2;
	const float duration{ m_action_config.trans_in_out_duration_stage_2 };
	m_state_countdown.Initialize(duration);
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->EnterRoomStateTransInSecond(duration);
	}
}

void Room::UpdateRoomStateTransInSecond()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateActive();
		return;
	}
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->UpdateRoomStateTransInSecond();
	}
}

void Room::EnterRoomStateActive()
{
	m_state = RoomState::ACTIVE;
	auto player = m_player.lock();
	player->ResetDefaultStates();
	for (auto& room_object : m_room_objects)
	{
		room_object.lock()->EnterRoomStateActive();
	}
}

void Room::UpdateRoomStateActive()
{
	if (CheckPlayerDead())
	{
		auto field = m_field.lock();
		field->StartFade();
		RestoreCheckPointStates();
	}

	if (CheckWaveClearState())
	{
		InitializeNextWave();
	}

	if (CheckClearState())
	{
		if (IfLastRoom())
		{
			auto field = m_field.lock();
			field->EnterExit();
			EnterRoomStateDone();
		}
		else
		{
			EnterRoomStateTransOut();
		}
	}
}

void Room::EnterRoomStateTransOut()
{
	//// room clear hit stop
	//m_state = RoomState::TRANS_OUT_STAGE_1;
	//m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_1);
	//// room items trans out
	//m_bumper_manager.lock()->SetRoomTransOut(m_room_id, m_action_config.trans_in_out_duration_stage_1);

	////auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	////comp_render.SetActive(false);

	//// next room enter trans in
	//if (!IfLastRoom())
	//{
	//	m_field.lock()->SetTransferActiveRoom(m_room_id + 1);
	//}
}

void Room::UpdateRoomStateTransOutFirst()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateTransOutSecond();
		return;
	}
}

void Room::EnterRoomStateTransOutSecond()
{
	//m_state = RoomState::TRANS_OUT_STAGE_2;
	//m_state_countdown.Initialize(m_action_config.trans_in_out_duration_stage_2);
	//// parts
	//m_room_floor.lock()->EnterTransOut(m_action_config.trans_in_out_duration_stage_1);
}

void Room::UpdateRoomStateTransOutSecond()
{
	m_state_countdown.Update(GetDeltaTime());
	float t = m_state_countdown.GetT();
	if (t <= 0.0f)
	{
		EnterRoomStateDone();
		return;
	}
}

void Room::EnterRoomStateDone()
{
	m_state = RoomState::DONE;
	// disable components
	// auto& comp_render = m_components.Get<ComponentRendererMesh>(m_comp_id_render);
	// comp_render.SetActive(false);
	//m_room_floor.lock()->EnterDone();
}

const Player& Room::GetPlayer() const
{
	return *m_player.lock();
}

Player& Room::GetPlayer()
{
	return *m_player.lock();
}

bool Room::CheckWaveClearState() const
{
	if (m_wave_index < 0)
	{
		return true;
	}
	for (auto& enemy : m_enemies)
	{
		if (!enemy.lock()->IfDone())
		{
			return false;
		}
	}
	// TODO: other clear conditions...
	return true;
}

bool Room::CheckClearState() const
{
	if (m_wave_index >= m_config.enemy_waves.size())
	{
		return true;
	}
	// TODO: other clear conditions...
	return false;
}

bool Room::CheckPlayerDead() const
{
	auto player = m_player.lock();
	if (player->IfDead())
	{
		return true;
	}
	//auto& player_pos = player->GetTransform().GetPosition();
	//bool fall = player_pos.y < (m_config.floor.height - 10.0f);
	//return fall;
	return false;
}

void Room::InitializeNextWave()
{
	m_enemies.clear();
	++m_wave_index;
	if (m_wave_index >= m_config.enemy_waves.size())
	{
		return;
	}
	const auto& wave_config = m_config.enemy_waves[m_wave_index];
	for (const auto& enemy_prime_config : wave_config.m_enemies_prime)
	{
		auto enemy = GetOwner().CreateGameObject<EnemyPrime>();
		enemy->InitializeConfig(enemy_prime_config, std::dynamic_pointer_cast<Room>(shared_from_this()));
		enemy->EnterTransIn(2.0f);
		m_enemies.push_back(enemy);
	}
}

void Room::InitializeFloor()
{
	for (auto& floor_config : m_config.floors)
	{
		auto floor = GetOwner().CreateGameObject<RoomFloor>();
		floor->InitializeConfig(floor_config, std::dynamic_pointer_cast<Room>(shared_from_this()));
		m_room_objects.push_back(floor);
		m_floors.push_back(floor);
	}
}

void Room::InitializeDashItems()
{
	auto dash_items = GetOwner().CreateGameObject<RoomDashItems>();
	dash_items->InitializeConfig(std::dynamic_pointer_cast<Room>(shared_from_this()));
	m_room_objects.push_back(dash_items);
	m_dash_items = dash_items;
}

void Room::InitializeBackground()
{
	auto background = GetOwner().CreateGameObject<RoomBackground>();
	background->InitializeConfig("stage_02"); // TODO config
}

bool Room::IfFirstRoom() const
{
	return m_room_id == 0;
}

bool Room::IfLastRoom() const
{
	return m_room_id == m_field_config.rooms.size() - 1;
}
