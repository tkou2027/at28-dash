#pragma once
#include "object/game_object.h"
#include "game_util/enemy_config.h"
#include "util/countdown_timer.h"

class EnemyAttackPhaseObject : public GameObject
{
public:
	virtual void EnterAttackPrepare(float duration) {}
	virtual void UpdateAttackPrepare() {}

	virtual void EnterAttackFollow(float duration) {}
	virtual void UpdateAttackFollow() {}

	virtual void EnterAttackPreExecute(float duration) {}
	virtual void UpdateAttackPreExecute() {}
	
	virtual void EnterAttackExecute(float duration) {}
	virtual void UpdateAttackExecute() {}

	virtual void EnterAttackCooldown(float duration) {}
	virtual void UpdateAttackCooldown() {}
	virtual void ExitAttackCooldown() {}
};

class EnemyAttackPhasePatternBase : public EnemyAttackPhaseObject
{
public:
	// lifecycle forwarded to children
	void EnterAttackPrepare(float duration) override;
	void UpdateAttackPrepare() override;

	void EnterAttackFollow(float duration) override;
	void UpdateAttackFollow() override;

	void EnterAttackPreExecute(float duration) override;
	void UpdateAttackPreExecute() override;

	void EnterAttackExecute(float duration) override;
	void UpdateAttackExecute() override;

	void EnterAttackCooldown(float duration) override;
	void UpdateAttackCooldown() override;
	void ExitAttackCooldown() override;

	const EnemyAttackConfig& GetAttackConfig() const { return m_attack_config; }
protected:
	std::vector<std::weak_ptr<class EnemyAttackPhaseObject> > m_attack_phase_objects;
	// helper to manage children
	void ClearAttackPhaseObjects();
	CountdownTimer m_attack_phase_timer{};
	EnemyAttackConfig m_attack_config{};
};

class EnemyAttackPhasePatternAA : public EnemyAttackPhasePatternBase
{
public:
	void Initialize() override;
	void EnterAttackPrepare(float duration) override;
	void EnterAttackFollow(float duration) override;
	void EnterAttackPreExecute(float duration) override;
	void EnterAttackExecute(float duration) override;
	void EnterAttackCooldown(float duration) override;
	void ExitAttackCooldown() override;
	// game states
	void UpdatePatternIndex();
	void RestoreCheckPointStates();
private:
	void CreateItems();
	void DestroyItems();
	void ClearItems();

	float m_range{ 80.0f }; // TODO
	int m_pattern_index{ -1 };
	std::weak_ptr<class Player> m_player;
	std::vector<std::weak_ptr<class DashItem> > m_dash_items;

	std::vector<EnemyAttackConfig> m_attack_config_presets{};
};