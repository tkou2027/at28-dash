#pragma once
#include <vector>
#include "dash_item_config.h"

enum class EnemyAttackState
{
	IDLE,
	PREPARE,
	FOLLOW,
	PRE_EXECUTE,
	EXECUTE,
	COOLDOWN
};

enum class EnemyAttackType
{
	ITEM
};

struct EnemyAttackItemConfig
{
	Vector3 position{};
	float size{ 15.0f };
	float size_max{ 15.0f };
	bool delay{ false };
	float index_size{ 1.0f };
	float index_pos_y{ 2.0f };
};

struct EnemyAttackConfig
{
	// action
	EnemyAttackType type{ EnemyAttackType::ITEM };
	std::vector<DashItemConfig> items{};
	std::vector<EnemyAttackItemConfig> attack_items{};

	float prepare_duration{ 0.1f };
	float follow_duration{ 1.0f };
	float pre_execute_duration{ 0.1f };
	float execute_duration{ 2.0f };
	float cooldown_duration{ 15.0f };
};

struct EnemyAttackPhaseConfig
{
	std::vector<EnemyAttackConfig> attacks{};
};

struct EnemyPrimeConfig
{
	// shape
	Vector3 position{ 0.0f, 0.0f, 0.0f };
	float size{ 30.0f };
	// ui
	Vector2 health_bar_size{ 24.0f, 400.0f };
	float health_bar_offset_x{ 80.0f };
	// status
	float health{ 90.0f };
	// action
	float damage_delay{ 1.0f };
	float exit_duration{ 1.0f };
	// attacks
	std::vector<EnemyAttackPhaseConfig> attack_phases{};
};

struct EnemyWaveConfig
{
	std::vector<EnemyPrimeConfig> m_enemies_prime;
};
