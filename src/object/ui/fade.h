#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"

class Fade : public GameObject
{
public:
	struct FadeConfig
	{
		float duration{ 0.5f };
	};
	void InitializeConfig(const FadeConfig& config);
	void Initialize() override;
	void Update() override;
	void StartFadeIn(bool fade_in_out = false);
	void StartFadeOut(bool fade_out_in = false);
	bool IsIdle() const;
private:
	static constexpr int NUM_SPRITES{ 2 };
	enum class FadeState
	{
		IDLE,
		FADE_IN,
		FADE_OUT
	};
	void EnterIdle();
	void UpdateFadeIn();
	void UpdateFadeOut();
	CountdownTimer m_countdown{};
	ComponentId m_comp_id_render{};
	FadeState m_state{ FadeState::IDLE };
	FadeConfig m_config{};
	bool m_fade_in_out{ false };
	bool m_fade_out_in{ false };
};