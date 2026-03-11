#pragma once
#include "object/game_object.h"
#include "util/yoyo_timer.h"

class LockOnUI : public GameObject
{
public:
	struct LockOnUiConfig
	{
		float size_min{};
		float size_max{};
		float animation_interval{};
	};
	int GetOrCreateMaterialDesc() const;
	void Initialize() override;
	void InitializeConfig(const LockOnUiConfig& config);
	void Update() override;
	void SetVisible(bool visible);
	void SetAnimationActive(bool active);
	void EnterExit();
private:
	void EnterIdle();
	void EnterActive();
	void UpdateActive();
	YoyoTimer m_animation_timer{};
	ComponentId m_comp_id_render{};
	enum class State
	{
		IDLE,
		ACTIVE,
		EXIT
	};
	State m_state{ State::IDLE };
	LockOnUiConfig m_config{};
};