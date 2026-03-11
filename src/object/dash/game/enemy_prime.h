#pragma once
#include "object/game_object.h"
#include "game_util/enemy_config.h"
#include "util/countdown_timer.h"

class EnemyPrime : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(const EnemyPrimeConfig& enemy_config, std::weak_ptr<class Room> room);
	void Update() override;
	bool IfDone() const { return m_state == EnemyState::DONE; }

	void EnterTransIn(float duration);
	void EnterAttack();
	void OnDamaged(float damage, const Vector3& position);
	void RestoreCheckPointStates();
private:
	void InitializeCollision();
	void InitializeVisual();
	void InitializeStatus();

	// trans in
	void UpdateTransIn();
	// attack
	void UpdateAttack();
	void EnterAttackPrepare();
	void UpdateAttackPrepare();
	void EnterAttackFollow();
	void UpdateAttackFollow();
	void EnterAttackPreExecute();
	void UpdateAttackPreExecute();
	void EnterAttackExecute();
	void UpdateAttackExecute();
	void EnterAttackCooldown();
	void UpdateAttackCooldown();
	void ExitAttackCooldown();

	void CheckAttackPhaseTransition() {}
	void UpdateNextAttack();
	// hurt
	void EnterHurt();
	void UpdateHurt();
	// collision
	void UpdateCollision();

	// follow
	//void UpdateAimCameraFollow(float offset = 1.0f);
	//void UpdateAimUIFollow();
	// aim utils
	//void UpdateAimVisuals();
	// dash items
	void CreateAttackItems();
	void DestroyItems();
	// exit
	void EnterExit();
	void UpdateExit();

	ComponentId m_comp_id_collider{};
	// references
	std::weak_ptr<class Room> m_room;
	std::weak_ptr<class Player> m_player;
	std::weak_ptr<class TimeController> m_time_controller;
	std::weak_ptr<class HitStopUpdater> m_hit_stop_updater;
	std::vector<std::weak_ptr<class DashItem> > m_active_items;
	// std::vector<std::weak_ptr<class EnemyAttackItem> > m_attack_items;
	std::vector<std::weak_ptr<class EnemyAttackPhaseObject> > m_attack_objects;
	// parts
	std::weak_ptr<class EnemyPrimeUI> m_status_ui;
	std::weak_ptr<class EnemyAttackPhasePatternAA> m_attack_pattern_curr; // TODO
	EnemyAttackConfig m_attack_config;
	// std::vector<std::weak_ptr<class EnemyAttackPhasePatternBase> > m_attack_patterns;

	// model
	ComponentId m_comp_id_render_mesh{};
	int m_id_mesh_aim{ -1 };
	int m_id_mesh_shell{ -1 };
	int m_id_mesh_core{ -1 };
	int m_id_mesh_border{ -1 };
	int m_id_collider_shell{ -1 };
	int m_id_collider_core{ -1 };

	enum class EnemyState
	{
		IDLE,
		TRANS_IN,
		// active states
		ATTACK,
		HURT,
		EXIT,
		DONE
	};

	struct EnemyStatus
	{
		float health{};
		CountdownTimer state_countdown{};
		// attack
		int attack_phase_index{ -1 };
		int attack_index{ -1 };
		EnemyAttackState attack_state{ EnemyAttackState::IDLE };
		CountdownTimer attack_countdown{};
	};
	EnemyState m_state{};
	EnemyStatus m_status{};

	EnemyPrimeConfig m_config{};
};