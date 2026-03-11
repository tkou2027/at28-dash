#pragma once
#include "math/vector2.h"

class PlayerInput
{
public:
	static Vector2 GetInputMove();
	static bool GetInputShoot();
	static bool GetInputDash();
};