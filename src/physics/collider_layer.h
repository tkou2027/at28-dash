#pragma once
#include <cstdint>

namespace ColliderLayer
{
	enum class Type : int
	{
		DEFAULT = 0,
		PLAYER,
		GROUND,
		BORDER,
		DASH_ITEM,
		GOAL,
		ENEMY,
		ENEMY_ATTACK,
		ENEMY_SHELL,
		ENEMY_CORE,

		//PLAYER_DASH, // special layer for player dash state
		//BUMPER_LOCKON, // special layer for player dash state
		// count
		MAX
	};

	enum Mask : uint32_t
	{
		MASK_NONE = 0,

		MASK_DEFAULT = 1 << static_cast<int>(Type::DEFAULT),
		MASK_PLAYER = 1 << static_cast<int>(Type::PLAYER),
		MASK_GROUND = 1 << static_cast<int>(Type::GROUND),
		MASK_BORDER = 1 << static_cast<int>(Type::BORDER),
		MASK_DASH_ITEM= 1 << static_cast<int>(Type::DASH_ITEM),
		MASK_GOAL= 1 << static_cast<int>(Type::GOAL),
		MASK_ENEMY = 1 << static_cast<int>(Type::ENEMY),
		MASK_ENEMY_ATTACK = 1 << static_cast<int>(Type::ENEMY_ATTACK),
		MASK_ENEMY_SHELL = 1 << static_cast<int>(Type::ENEMY_SHELL),
		MASK_ENEMY_CORE = 1 << static_cast<int>(Type::ENEMY_CORE),

		MASK_All = 0xffffffff,
	};
}
