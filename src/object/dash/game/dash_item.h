#pragma once
#include "object/game_object.h"
#include "game_util/dash_item_config.h"
#include "util/countdown_timer.h"


class DashItem : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void InitializeConfig(const DashItemConfig& config);
	void OnDamage(float damage);
	DashItemTriggerInfo OnTrigger();
	void EnterTriggeringBack();
	void EnterDestroying();
	void SetUsed();
private:
	void InitializeVisual();
	void InitializeCollision();
	void EnterActive();
	void EnterHurt();
	void UpdateHurt();
	void EnterDead();
	void EnterTriggering();
	void UpdateTriggering();
	void EnterTriggerDone();
	void UpdateTriggeringBack();
	void UpdateDestroying();
	void EnterDone();

	enum class DashItemState
	{
		IDLE,
		ACTIVE,
		HURT,
		DEAD,
		TRIGGERING,
		TRIGGER_DONE,
		TRIGGERING_BACK,
		DESTROYING,
		DONE
	};
	struct DashItemStatus
	{
		float hp{};
		bool registered{ false }; // dead, can dash, remove from list when destroy
		bool used{ false }; // player starts dashing, don't destroy
		CountdownTimer state_countdown{};
	};
	DashItemConfig m_config{};
	DashItemStatus m_status{};
	DashItemState m_state{ DashItemState::ACTIVE };
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_collider{};
	int m_id_mesh_active{ -1 };
	int m_id_mesh_border{ -1 };
	int m_id_mesh_dead{ -1 }; // mask
};