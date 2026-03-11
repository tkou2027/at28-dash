#include "enemy_prime_ui.h"

#include "object/game_object_list.h"
#include "object/dash/common/value_bar.h"
#include "config/camera_names.h"
#include "config/palette.h"

void EnemyPrimeUI::Initialize() {}

void EnemyPrimeUI::InitializeConfig(const EnemyPrimeConfig& config)
{
	m_health_full = config.health;

	auto health_bar = GetOwner().CreateGameObject<ValueBar>();
	ValueBar::ValueBarConfig health_bar_config{};
	health_bar_config.size = config.health_bar_size;
	health_bar_config.delay_duration = config.damage_delay;
	health_bar_config.color = g_palette.color_enemy_core;
	// TODO: config
	float bar_x = g_camera_presets.main.width - config.health_bar_size.x * 0.5f - config.health_bar_offset_x;
	float bar_y = g_camera_presets.main.height * 0.5f;
	health_bar_config.position = { bar_x, bar_y }; 
	health_bar_config.value_full = config.health;
	health_bar->InitializeConfig(health_bar_config);
	health_bar->SetValueNoDelay(0.0f);
	health_bar->SetValue(false);
	m_health_bar = health_bar;
}

void EnemyPrimeUI::SetValue(float value)
{
	auto health_bar = m_health_bar.lock();
	assert(health_bar);
	health_bar->SetValue(value);
}

void EnemyPrimeUI::EnterActive()
{
	auto health_bar = m_health_bar.lock();
	assert(health_bar);
	health_bar->SetVisible(true);
	health_bar->SetValue(m_health_full);
}
