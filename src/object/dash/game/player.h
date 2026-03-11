#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"
#include "physics/collider_layer.h"
#include "object/dash/game/game_util/player_animation_index.h"

class Player : public GameObject
{
public:;
	void Initialize() override;
	void OnSceneInitialized() override;
	void Update() override;
	void OnCollision(const struct CollisionInfo& collision) override;
	void OnTrigger(const struct CollisionInfo& collision) override;
	// states public
	void ResetDefaultStates();
	void EnterMoveDefault();
	void EnterControlled();
	bool IfDead() const;
private:
	// states
	// move
	void UpdateMove();
	void CheckEnterDash();
	void UpdateDash();
	// shoot state
	void UpdateShoot();
	// hurt state
	void EnterHurt();
	void UpdateHurt();
	void ResetHurtEffects();

	// collision
	void OnTriggerCancelOut(const struct CollisionInfo& collision);
	void OnTriggerEnemyAttack(const struct CollisionInfo& collision);

	void HitEffect(ColliderLayer::Type layer);

	// input 
	Vector3 GetMoveInput() const;
	// move utils
	void RotateToMoveDirection(const Vector3& move_dir);

	// animation utils
	void SetAnimationIndex(PlayerAnimationIndex index);

	// components ========
	ComponentId m_comp_id_render;
	ComponentId m_comp_id_collider;
	// references ========
	std::weak_ptr<class CameraFollow> m_camera{};
	std::weak_ptr<class HitStopUpdater> m_hit_stop_updater{};
	std::weak_ptr<class Field> m_field{};
	// parts =======
	std::weak_ptr<class PlayerBullets> m_bullets{};
	std::weak_ptr<class PlayerAim> m_aim{};

	// move config ========
	struct MoveConfig
	{
		// default moving
		float max_speed_default{ 0.5f };
		float max_acc_default{ 0.02f };
		// dash and bump
		float dash_duration{ 1.0f / 6.0f };
		// dash speed
		int speed_level_max{ 5 };
		float distance_per_speed_level{ 8.0f };
		float speed_level_drop_interval{ 0.5f };
	};
	MoveConfig m_move_config{};

	// states ========
	enum class MoveState
	{
		DEFAULT,
		DASH,
		CONTROLLED
	};
	MoveState m_state{ MoveState::DEFAULT };
	CountdownTimer m_state_countdown{};

	enum class HurtState
	{
		NONE,
		HURT,
		DEAD
	};
	struct StatusConfig
	{
		float max_hp{ 1.0f };
		float hurt_duration{ 1.0f };
	};
	StatusConfig m_status_config{};
	HurtState m_hurt_state{ HurtState::NONE };
	CountdownTimer m_hurt_countdown{};
	float m_hp{ 1.0f };

	// move state
	CountdownTimer m_speed_level_timer{};
	Vector3 m_desired_velocity{};
	int m_speed_level{ 0 };

	// dash state
	Vector3 m_dash_init_pos;
	Vector3 m_dash_target_pos;
	float m_dash_countdown{ 0.0f };

	// shoot state
	struct ShootConfig
	{
		float cooldown{ 0.2f };
		float cooldown_time_stop{ 0.1f };
		float shoot_speed{ 60.0f };
		Vector3 shoot_offset{ 0.0f, 1.5f, 0.0f };
	};
	ShootConfig m_shoot_config{};
	CountdownTimer m_shoot_cooldown_timer{};

	// goal state
	Vector3 m_goal_target_pos;
};