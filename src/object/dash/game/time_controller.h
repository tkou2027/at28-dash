#pragma once
#include <memory>
#include "object/game_object.h"
#include "math/vector3.h"
#include "util/countdown_timer.h"

class TimeController : public GameObject
{
public:
	void OnSceneInitialized() override;
    void Update() override;
	void EnterCountdown(float duration);
	float GetTimeScale() const { return m_time_scale; }
	bool IfInCountdownOrRestore() const
	{ return m_state == TimeState::COUNTDOWN || m_state == TimeState::RESTORE; }
	bool IfInCountdown() const { return m_state == TimeState::COUNTDOWN; }
	void Reset();
private:
    enum class TimeState
    {
        DEFAULT,
        COUNTDOWN,
        RESTORE,
    };
    struct TimerConfig
    {
        float countdown_duration{ 3.0f };
		float restore_duration{ 1.0f };
	};
    TimerConfig m_config{};
    void UpdateCountdown();
	void EnterRestore();
	void UpdateRestore();
	void EnterDefault();
	CountdownTimer m_state_timer{};
	TimeState m_state{ TimeState::DEFAULT };
	float m_time_scale{ 1.0f };
	int m_tick{ 0 };
	std::weak_ptr<class DashController> m_dash_controller{};
};
