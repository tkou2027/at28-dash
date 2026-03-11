#pragma once
#include "math/vector4.h"

struct Palette
{
	Vector4 color_theme{};
	Vector4 color_theme_accent{};
	Vector4 color_player{};
	Vector4 color_enemy_shell{};
	Vector4 color_enemy_core{};
	Vector4 color_enemy_hurt{};
	Vector4 color_dash_item{};
};

extern Palette g_palette;