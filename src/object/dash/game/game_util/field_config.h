#pragma once
#include <vector>
#include "math/vector2.h"
#include "math/vector3.h"
#include "dash_item_config.h"
#include "enemy_config.h"

struct FloorConfig
{
	Vector3 position{};
	int block_num_x{ 0 };
	int block_num_z{ 0 };
	float block_scale{ 1.0f };
};

struct RoomConfig
{
	std::vector<FloorConfig> floors;
	std::vector<DashItemConfig> dash_items;
	std::vector<EnemyWaveConfig> enemy_waves;
};

struct FieldConfig
{
	std::vector<RoomConfig> rooms{};
};

extern FieldConfig g_field_config;