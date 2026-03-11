#pragma once
#include "object/game_object.h"
#include "game_util/field_config.h"
#include "util/countdown_timer.h"

class DashController;
class TimeController;
class Field : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void OnSceneInitialized() override;
	void InitializeConfig(const FieldConfig& config);
	void SetInitiateActiveRoom(int room_id);
	void SetTransferActiveRoom(int room_id);
	void EnterExit();
	bool IfDone() const;
	void StartFade();
	DashController& GetDashController() const;
	TimeController& GetTimeController() const;
private:
	enum class FieldState
	{
		IDLE,
		ACTIVE,
		EXIT,
		DONE
	};
	FieldConfig m_config{};
	// references
	std::weak_ptr<class Player> m_player{};
	std::weak_ptr<class CameraFollow> m_camera{};
	// parts
	std::weak_ptr<DashController> m_dash_controller{};
	std::weak_ptr<TimeController> m_time_controller{};
	std::vector<std::weak_ptr<class Room> > m_rooms;
	std::weak_ptr<class Fade> m_fade{};
	int m_current_room_id{ -1 };

	FieldState m_state{ FieldState::IDLE };
};