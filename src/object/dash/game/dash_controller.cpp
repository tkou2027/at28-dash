#include "dash_controller.h"
#include "dash_item.h"
#include "field.h"
#include "time_controller.h"
#include "global_context.h"
#include "platform/sound.h"

void DashController::Initialize()
{
	m_dash_items.clear();
}

void DashController::SetField(std::weak_ptr<Field> field)
{
	m_field = field;
}

void DashController::AddDashItem(std::weak_ptr<DashItem> dash_item)
{
	m_dash_items.push_back(dash_item);
}

void DashController::RemoveDashItem(std::weak_ptr<DashItem> dash_item)
{
	m_dash_items.erase(std::remove_if(m_dash_items.begin(), m_dash_items.end(),
		[&dash_item](const std::weak_ptr<DashItem>& item) {
			auto item_shared = item.lock();
			auto dash_item_shared = dash_item.lock();
			return item_shared && dash_item_shared && item_shared == dash_item_shared;
		}), m_dash_items.end());
}

bool DashController::IfCanDash() const
{
	return !m_dash_items.empty();
}

Vector3 DashController::GetAndAssureDashTarget() const
{
	assert(!m_dash_items.empty());
	auto dash_item = m_dash_items.front().lock();
	assert(dash_item);
	dash_item->SetUsed();
	return dash_item->GetTransform().GetPositionGlobal();
}

void DashController::TriggerDashItem()
{
	assert(!m_dash_items.empty());
	auto dash_item = m_dash_items.front().lock();
	assert(dash_item);
	// TODO: trigger effect
	auto& time_controller = m_field.lock()->GetTimeController();
	
	m_dash_item_active = m_dash_items.front();
	m_dash_items.pop_front();
	const auto& trigger_info = dash_item->OnTrigger();
	if (!trigger_info.trigger_time)
	{
		return;
	}
	// effect
	auto& sound = g_global_context.m_sound;
	sound->SetPitchAll(0.87f);
	sound->SetMuffledFilterAll(0.1f);
	sound->SetReverbLevelAll(1.0f);
	sound->SetVolumeAll(2.5f);

	time_controller.EnterCountdown(trigger_info.trigger_time_duration);
}

void DashController::ReleaseDashItem()
{
	auto dash_item_active = m_dash_item_active.lock();
	assert(dash_item_active);
	dash_item_active->EnterTriggeringBack();
	m_dash_item_active.reset();

	// effect
	auto& sound = g_global_context.m_sound;
	sound->SetPitchAll(1.0f);
	sound->ResetFilterAll();
	sound->ResetReverbLevelAll();
	sound->SetVolumeAll(1.0f);
}

void DashController::Reset()
{
	m_dash_items.clear();
	m_dash_item_active.reset();
}