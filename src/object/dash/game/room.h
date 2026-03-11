#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"
#include "game_util/field_config.h"

class Player;
class Field;
class Room : public GameObject
{
public:
	enum class RoomState
	{
		IDLE,
		TRANS_IN_STAGE_1, // floor
		TRANS_IN_STAGE_2, /// player & item
		ACTIVE,
		TRANS_OUT_STAGE_1, // player move
		TRANS_OUT_STAGE_2, // fall
		DONE
	};
	void Initialize() override;
	void OnSceneInitialized() override;
	void InitializeConfig(const FieldConfig& field_config, int id,
		std::weak_ptr<Field> field);
	void Update() override;
	// for parts
	const RoomConfig& GetConfig() const { return m_config; }
	const class Player& GetPlayer() const;
	class Player& GetPlayer();
	RoomState GetState() const { return m_state; }
	// states
	void EnterRoomStateIdle();
	void EnterRoomStateTransIn();
	void RestoreCheckPointStates();
private:
	void UpdateRoomStateTransInFirst();
	void EnterRoomStateTransInSecond();
	void UpdateRoomStateTransInSecond();
	void EnterRoomStateActive();
	void UpdateRoomStateActive();
	void EnterRoomStateTransOut();
	void UpdateRoomStateTransOutFirst();
	void EnterRoomStateTransOutSecond();
	void UpdateRoomStateTransOutSecond();
	void EnterRoomStateDone();

	bool CheckWaveClearState() const;
	bool CheckClearState() const;
	bool CheckPlayerDead() const;
	void InitializeNextWave();
private:
	// parts initialization
	void InitializeFloor();
	void InitializeDashItems();
	void InitializeBackground();

	bool IfFirstRoom() const;
	bool IfLastRoom() const;

	// config
	FieldConfig m_field_config{};
	int m_room_id{};
	RoomConfig m_config{};
	struct RoomActionConfig
	{
		float trans_in_out_duration_stage_1{ 0.1f };
		float trans_in_out_duration_stage_2{ 0.2f };
	};
	RoomActionConfig m_action_config{};
	// state
	RoomState m_state{ RoomState::IDLE };
	CountdownTimer m_state_countdown{};
	int m_wave_index{ -1 };
	// references
	std::weak_ptr<class Field> m_field{};
	std::weak_ptr<class Player> m_player{};
	std::weak_ptr<class CameraFollow> m_camera{};
	// parts
	std::vector<std::weak_ptr<class RoomObject> > m_room_objects{};
	std::vector<std::weak_ptr<class RoomFloor> > m_floors{};
	std::weak_ptr<class RoomDashItems> m_dash_items{};
	// enemies this wave
	std::vector <std::weak_ptr<class EnemyPrime> > m_enemies{};
};