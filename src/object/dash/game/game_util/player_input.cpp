#include "player_input.h"

#include "global_context.h"
#include "platform/controller.h"
#include "platform/keyboard.h"

Vector2 PlayerInput::GetInputMove()
{
	// controller
	auto& controller = g_global_context.m_controller;
	Vector2 input{ controller->GetLeftX(), controller->GetLeftY() };
	
	// keyboard
	if (g_global_context.m_keyboard->IsKeyDown(KK_LEFT))
	{
		input.x -= 1.0f;
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_RIGHT))
	{
		input.x += 1.0f;
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_UP))
	{
		input.y += 1.0f;
	}
	if (g_global_context.m_keyboard->IsKeyDown(KK_DOWN))
	{
		input.y -= 1.0f;
	}

	// magnitude clamp
	input.ClampMagnitude(1.0f);
	return input;
}

bool PlayerInput::GetInputShoot()
{
	// controller
	auto& controller = g_global_context.m_controller;
	if (controller->IsDown(Controller::Button::LB)
		|| controller->IsDown(Controller::Button::RB)
		|| controller->IsDown(Controller::Button::LS)
		|| controller->IsDown(Controller::Button::RS))
	{
		return true;
	}

	// keyboard
	return g_global_context.m_keyboard->IsKeyDown(KK_Q);
}

bool PlayerInput::GetInputDash()
{
	// controller
	auto& controller = g_global_context.m_controller;
	if (controller->IsPressed(Controller::Button::B)
		|| controller->IsPressed(Controller::Button::A))
	{
		return true;
	}
	// keyboard
	return g_global_context.m_keyboard->IsKeyTrigger(KK_SPACE);
}