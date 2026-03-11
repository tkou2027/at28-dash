#pragma once
#include "object/game_object.h"
#include <array>

struct BulletConfig
{
	float speed{ 0.0f };
	float life{ 40.0f };
	Vector3 direction{};
	Vector3 origin{};
};

class PlayerBullets;

class PlayerBulletInstance
{
public:
	void Initialize(const BulletConfig& config, int index, PlayerBullets* owner);
	bool IfUse() const { return m_life > 0.0f; }
	void Reset() { m_life = -1.0f; }
	void Update();
	void Destroy();
private:
	BulletConfig m_config{};
	float m_life{ -1.0f };
	int m_index{ -1 };
	Vector3 m_position{};
	PlayerBullets* m_owner{ nullptr };
};

class PlayerBullets : public GameObject
{
	friend class PlayerBulletInstance;
public:
	void Initialize() override;
	void OnSceneInitialized() override;
	void Update() override;
	void CreateBullet(const BulletConfig& config);
	void ClearBullets();
private:
	static constexpr int MAX_BULLET{ 512 };
	std::array<PlayerBulletInstance, MAX_BULLET> m_bullets{};
	ComponentId m_comp_id_render_mesh{};
	std::weak_ptr<class TimeController> m_time_controller{};
};