#pragma once

enum class DashItemUIType
{
	NONE,
	INPUT
};

struct DashItemConfig
{
	// shape
	float radius{ 8.0f * Math::INV_SQRT_2 };
	float height{ 8.0f };
	float inner_ratio{ 0.8f };
	Vector3 position{};
	// status
	float hp_max{ 5.0f };
	float trigger_scale_max{ 100.0f };
	float trigger_duration{ 0.5f };
	float hurt_duration{ 0.1f };
	float destroy_duration{ 0.2f };
	DashItemUIType ui_type{ DashItemUIType::NONE };
};

struct DashItemTriggerInfo
{
	bool trigger_time{ true };
	float trigger_time_duration{ 3.0f };

};