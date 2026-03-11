#include "time_controller.h"

#include "object/game_object_list.h"
#include "object/dash/game/dash_controller.h"

#include "global_context.h"
#include "platform/sound.h"
#include "config/audio_manager.h"

void TimeController::OnSceneInitialized()
{
	m_dash_controller = GetOwner().FindGameObject<DashController>();
	assert(!m_dash_controller.expired());
}

void TimeController::Update()
{
	switch (m_state)
	{
	case TimeState::COUNTDOWN:
	{
		UpdateCountdown();
		break;
	}
	case TimeState::RESTORE:
	{
		UpdateRestore();
		break;
	}
	}
}

void TimeController::EnterCountdown(float duration)
{
	m_config.countdown_duration = duration;
	m_state_timer.Initialize(m_config.countdown_duration);
	m_state = TimeState::COUNTDOWN;
}

void TimeController::UpdateCountdown()
{
	float t = m_state_timer.Update(GetDeltaTime());
	//float count = m_state_timer.GetCount();
	//if (m_config.countdown_duration - count > m_tick)
	//{
	//	++m_tick;
	//	auto dash_controller = m_dash_controller.lock();
	//	dash_controller->TriggerDashItem();
	//	int se = g_global_context.m_audio_manager->LoadSoundEffect(SoundEffectName::COUNTDOWN);
	//	g_global_context.m_sound->PlaySoundLoop(se, 0);
	//}
	if (t <= 0.0f)
	{
		EnterRestore();
		return;
	}
	m_time_scale = 0.0f; // TODO: curve
}

void TimeController::EnterRestore()
{
	m_state = TimeState::RESTORE;
	m_time_scale = 1.0f;
	m_state_timer.Initialize(m_config.restore_duration);
}

void TimeController::UpdateRestore()
{
	float t = m_state_timer.Update(GetDeltaTime());
	if (t <= 0.0f)
	{
		EnterDefault();
		return;
	}
	m_time_scale = 1.0f; // TODO: curve
}

void TimeController::EnterDefault()
{
	m_state = TimeState::DEFAULT;
	m_time_scale = 1.0f;
	auto dash_controller = m_dash_controller.lock();
	dash_controller->ReleaseDashItem();
}

void TimeController::Reset()
{
	m_state = TimeState::DEFAULT;
	m_time_scale = 1.0f;
}