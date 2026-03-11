#pragma once
#include "object/game_object.h"

class PlayerAim : public GameObject
{
	friend class Player;
public:
	struct PlayerAimConfig
	{
		float line_width{ 1.0f };
		float gap{ 12.0f };
	};
	void Initialize() override;
	void OnSceneInitialized() override;
	void InitializeConfig(std::weak_ptr<Player> player);
	void Update() override;
	int GetOrCreateMaterialDesc() const;
private:
	static constexpr int MAX_SEGMENTS{ 8 };
	std::weak_ptr<class Player> m_player;
	std::weak_ptr<class DashController> m_dash_controller;
	std::vector<std::weak_ptr<class LockOnUI> > m_lock_on_uis;
	ComponentId m_comp_id_render_mesh{};
	PlayerAimConfig m_config{};
};