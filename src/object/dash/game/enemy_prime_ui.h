#pragma once
#include "object/game_object.h"
#include "object/dash/game/game_util/enemy_config.h"

class EnemyPrimeUI : public GameObject
{
public:
    void Initialize() override;
	void InitializeConfig(const EnemyPrimeConfig& config);
	void SetValue(float value);
	void EnterActive();
private:
    std::weak_ptr<class ValueBar> m_health_bar;
	float m_health_full{};
};
