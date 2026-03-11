#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class ValueBar : public GameObject
{
public:
	struct ValueBarConfig
	{
		// layout
		Vector2 size{};
		Vector2 position{};
		bool vertical{ true };
		bool dir_positive{ true };
		Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		// action
		float value_full{ 1.0f };
		float delay_duration{ 0.3f };
	};
	void Initialize() override;
	void InitializeConfig(const ValueBarConfig& config);
	void Update() override;
	void SetValue(float value);
	void SetValueNoDelay(float value);
	void SetVisible(bool visible);
private:
	void UpdateDelay();
	void InitializeSprites();
	void InitializeBarSize(int bar_id);
	void SetBarSizeByValue(int bar_id, float value);

	ComponentId m_comp_id_sprite{};

	// status
	enum class State
	{
		IDLE,
		ACTIVE,
		DELAY
	};
	State m_state{ State::ACTIVE };
	float m_value_prev{};
	float m_value_curr{};
	CountdownTimer m_delay_timer{};
	ValueBarConfig m_config{};
	// parts
	int id_bar_value{};
	int id_bar_back{};
};