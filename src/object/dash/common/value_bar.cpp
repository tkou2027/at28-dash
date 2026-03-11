#include "value_bar.h"
#include "component/render/component_renderer_sprite.h"
#include "config/camera_names.h"
#include "config/preset_manager.h"
#include "lockon_ui.h"

void ValueBar::Initialize()
{
	m_components.Add<ComponentRendererSprite>(m_comp_id_sprite);
}

void ValueBar::InitializeConfig(const ValueBarConfig& config)
{
	m_config = config;
	m_value_curr = config.value_full;
	m_transform_2d.SetPosition(m_config.position);
	InitializeSprites();
}

void ValueBar::Update()
{
	switch (m_state)
	{
	case State::IDLE:
	case State::ACTIVE:
	{
		break;
	}
	case State::DELAY:
	{
		UpdateDelay();
		break;
	}
	}
}

void ValueBar::SetValue(float value)
{
	value = Math::Clamp(value, 0.0f, m_config.value_full);
	if (value == m_value_curr)
	{
		return;
	}
	m_value_prev = m_value_curr;
	m_value_curr = value;
	m_state = State::DELAY;
	m_delay_timer.Initialize(m_config.delay_duration);
	// update value bar
	SetBarSizeByValue(id_bar_value, m_value_curr);
}

void ValueBar::SetValueNoDelay(float value)
{
	m_value_curr = value;
	m_value_prev = value;
	m_state = State::ACTIVE;
	SetBarSizeByValue(id_bar_back, m_value_curr);
	SetBarSizeByValue(id_bar_value, m_value_curr);
}

void ValueBar::SetVisible(bool visible)
{
	auto& comp_sprite = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	comp_sprite.SetActive(visible);
}

void ValueBar::UpdateDelay()
{
	float t = m_delay_timer.Update(GetDeltaTime());
	float value_delay = Math::Lerp(m_value_prev, m_value_curr, 1.0f - t);
	if (t <= 0.0f)
	{
		m_state = State::ACTIVE;
		value_delay = m_value_curr;
		// update sprites before return
	}
	// update back sprite
	SetBarSizeByValue(id_bar_back, value_delay);
}

void ValueBar::InitializeSprites()
{
	auto& preset_manager = GetPresetManager();
	auto& sprite = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	// bar back (add before value)
	{
		const auto& bar_back_desc = preset_manager.GetSpriteDesc("sprite/status/bar_back");
		Sprite bar_back_sprite{ bar_back_desc };
		bar_back_sprite.m_desc.color = m_config.color;
		id_bar_back = sprite.AddSprite(bar_back_sprite);
		InitializeBarSize(id_bar_back);
	}
	// bar value
	{
		const auto& bar_value_desc = preset_manager.GetSpriteDesc("sprite/status/bar_value");
		Sprite bar_value_sprite{ bar_value_desc };
		bar_value_sprite.m_desc.color = m_config.color;
		id_bar_value = sprite.AddSprite(bar_value_sprite);
		InitializeBarSize(id_bar_value);
	}
}

void ValueBar::InitializeBarSize(int bar_id)
{
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	auto& bar = sprite_comp.GetSprite(bar_id);
	bar.m_transform.SetParent(&m_transform_2d);
	bar.m_transform.SetScale(m_config.size);
	bar.m_transform.SetPosition(m_config.size * 0.5f);
	if (m_config.vertical)
	{
		bar.m_uv_animation_state.uv_scroll_size.y = m_config.size.y / m_config.size.x;
	}
	else
	{
		bar.m_uv_animation_state.uv_scroll_size.x = m_config.size.x / m_config.size.y;
	}
}

void ValueBar::SetBarSizeByValue(int bar_id, float value)
{
	auto& sprite_comp = m_components.Get<ComponentRendererSprite>(m_comp_id_sprite);
	auto& bar = sprite_comp.GetSprite(bar_id);
	float factor = m_config.value_full > 0.0f ? value / m_config.value_full : 1.0f;
	if (m_config.vertical)
	{
		const float length = m_config.size.y * factor;
		const float offset = (m_config.size.y - length) * (m_config.dir_positive ? 0.5f : -0.5f);
		bar.m_transform.SetScaleY(length);
		bar.m_transform.SetPositionY(offset);
	}
	else
	{
		const float length = m_config.size.x * factor;
		const float offset = (m_config.size.x - length) * (m_config.dir_positive ? 0.5f : -0.5f);
		bar.m_transform.SetScaleX(length);
		bar.m_transform.SetPositionX(offset);
	}
}
