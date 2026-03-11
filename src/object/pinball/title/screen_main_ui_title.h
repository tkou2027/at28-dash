#pragma once
#include "object/game_object.h"
#include "util/countdown_timer.h"
#include "object/pinball/game_util/enemy_config.h"

class ScreenMainUITitle : public GameObject
{
public:
	void Initialize() override;
	void Update() override;
	void Crush();
private:
	void UpdateDelay();
	void UpdateAnimation();
	void InitializeSprites();
	void SetSpritesSize();
	ComponentId m_comp_id_sprite{};
	ComponentId m_comp_id_mesh{};
};