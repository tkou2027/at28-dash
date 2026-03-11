#pragma once
#include "object/game_object.h"
#include "game_util/enemy_config.h"
#include "enemy_attack_phase_object.h"
#include "util/yoyo_timer.h"
#include "util/countdown_timer.h"

class EnemyAttackItem : public EnemyAttackPhaseObject
{
public:
	void Initialize() override;
	void InitializeConfig(const EnemyAttackItemConfig& config);
	void EnterAttackPreExecute(float duration) override;
	void UpdateAttackPreExecute() override;
	void EnterAttackExecute(float duration) override;
	void UpdateAttackExecute() override;
	void EnterAttackCooldown(float duration) override;
	void UpdateAttackCooldown() override;
	void ExitAttackCooldown() override;
private:
	void DoEnterPreExecuteCommon();
	void DoEnterPreExecute();
	void DoUpdatePreExecute();
	void DoEnterExecute();
	void DoUpdateExecute();
	void DoEnterCooldown();
	void InitializeVisual();
	void InitializeCollision();
	ComponentId m_comp_id_render{};
	ComponentId m_comp_id_collider{};
	int m_id_mesh_warn{};
	int m_id_mesh_attack{};
	int m_id_mesh_index{};
	int m_id_mesh_collider{};
	int m_id_collider_index{};
	int m_id_collider_attack{};

	
	EnemyAttackItemConfig m_config{};
	std::weak_ptr<class Room> m_room;
	YoyoTimer m_animation_timer{};
	CountdownTimer m_delay_timer{};
	bool m_done_execute{ false };
	bool m_dead{ false };
};