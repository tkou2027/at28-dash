#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"
#include "object/pinball/game_util/enemy_config.h"

class ScreenMainUI : public GameObject
{
public:
	struct AimInfo
	{
		float rotation_ratio{ 0.0f }; // u
		float height_ratio{}; // v
		float radius{};
		float thickness{};
	};
	void Initialize() override;
	void InitializeConfig(const EnemyCenterShapeConfig& shape_config);
	void Update() override;
	void SetAimInfo(const AimInfo& aim_info);
private:
	//void UpdateBorderAnimation();
	void UpdateDelay();
	void UpdateAnimation();
	void InitializeSprites();
	void SetSpritesSize();
	ComponentId m_comp_id_sprite{};
	ComponentId m_comp_id_mesh{};

	// status
	enum class State
	{
		IDLE,
		ACTIVE,
		DELAY
	};
	State m_state{ State::ACTIVE };
	CountdownTimer m_delay_timer{};
	CountdownTimer m_animation_timer{};

	struct ScreenMainUIConfig
	{
		float shape_radius;
		float shape_height;
	};
	ScreenMainUIConfig m_config{};
};