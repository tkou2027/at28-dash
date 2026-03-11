#include "enemy_attack_phase_object.h"
#include "object/game_object_list.h"
#include "object/dash/game/player.h"
#include "object/dash/game/enemy_attack_item.h"
#include "object/dash/game/dash_item.h"

// EnemyAttackPhasePatternBase -------------------------------------------------------------------

void EnemyAttackPhasePatternBase::EnterAttackPrepare(float duration)
{
	m_attack_phase_timer.Initialize(duration);
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->EnterAttackPrepare(duration);
		}
	}
}

void EnemyAttackPhasePatternBase::UpdateAttackPrepare()
{
	// advance timer
	m_attack_phase_timer.Update(GetDeltaTime());
	// forward update to children
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->UpdateAttackPrepare();
		}
	}
}

void EnemyAttackPhasePatternBase::EnterAttackFollow(float duration)
{
	m_attack_phase_timer.Initialize(duration);
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->EnterAttackFollow(duration);
		}
	}
}

void EnemyAttackPhasePatternBase::UpdateAttackFollow()
{
	m_attack_phase_timer.Update(GetDeltaTime());
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->UpdateAttackFollow();
		}
	}
}

void EnemyAttackPhasePatternBase::EnterAttackPreExecute(float duration)
{
	m_attack_phase_timer.Initialize(duration);
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->EnterAttackPreExecute(duration);
		}
	}
}

void EnemyAttackPhasePatternBase::UpdateAttackPreExecute()
{
	m_attack_phase_timer.Update(GetDeltaTime());
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->UpdateAttackPreExecute();
		}
	}
}

void EnemyAttackPhasePatternBase::EnterAttackExecute(float duration)
{
	m_attack_phase_timer.Initialize(duration);
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->EnterAttackExecute(duration);
		}
	}
}

void EnemyAttackPhasePatternBase::UpdateAttackExecute()
{
	m_attack_phase_timer.Update(GetDeltaTime());
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->UpdateAttackExecute();
		}
	}
}

void EnemyAttackPhasePatternBase::EnterAttackCooldown(float duration)
{
	m_attack_phase_timer.Initialize(duration);
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->EnterAttackCooldown(duration);
		}
	}
}

void EnemyAttackPhasePatternBase::UpdateAttackCooldown()
{
	m_attack_phase_timer.Update(GetDeltaTime());
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->UpdateAttackCooldown();
		}
	}
}

void EnemyAttackPhasePatternBase::ExitAttackCooldown()
{
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->ExitAttackCooldown();
		}
	}
}

void EnemyAttackPhasePatternBase::ClearAttackPhaseObjects()
{
	for (auto& obj : m_attack_phase_objects)
	{
		if (auto o = obj.lock())
		{
			o->SetObjectActive(false);
		}
	}
	m_attack_phase_objects.clear();
}

void EnemyAttackPhasePatternAA::Initialize()
{
	// TODO
	m_attack_config.prepare_duration = 1.0f;
	m_attack_config.follow_duration = 3.0f;
	m_attack_config.pre_execute_duration = 3.0f;
	m_attack_config.execute_duration = 1.0f;
	m_attack_config.cooldown_duration = 1.0f;

	m_attack_config_presets.clear();
	float size = m_range / 3;
	{
		EnemyAttackConfig attack{ m_attack_config };

		attack.type = EnemyAttackType::ITEM;
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ -size, 0.0f, 0.0f };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ size, 0.0f, 0.0f };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 1.2f;
			attack_item.position = Vector3{ 0.0f, 0.1f, 0.0f };
			attack_item.delay = true;
			attack.attack_items.push_back(attack_item);
		}
		m_attack_config_presets.push_back(attack);
	}

	{
		EnemyAttackConfig attack{ m_attack_config };

		attack.type = EnemyAttackType::ITEM;
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ 0.0f, 0.0f, -size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ 0.0f, 0.0f, size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 1.2f;
			attack_item.position = Vector3{ 0.0f, 0.1f, 0.0f };
			attack_item.delay = true;
			attack.attack_items.push_back(attack_item);
		}
		m_attack_config_presets.push_back(attack);
	}

	{
		EnemyAttackConfig attack{ m_attack_config };

		attack.type = EnemyAttackType::ITEM;
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ -size, 0.0f, -size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ size, 0.0f, size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 1.2f;
			attack_item.position = Vector3{ 0.0f, 0.1f, 0.0f };
			attack_item.delay = true;
			attack.attack_items.push_back(attack_item);
		}
		m_attack_config_presets.push_back(attack);
	}
	{
		EnemyAttackConfig attack{ m_attack_config };

		attack.type = EnemyAttackType::ITEM;
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ -size, 0.0f, -size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ size, 0.0f, size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ size, 0.0f, -size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ -size, 0.0f, size };
			attack.attack_items.push_back(attack_item);
		}
		{
			EnemyAttackItemConfig attack_item{};
			attack_item.size = size * 0.8f;
			attack_item.position = Vector3{ 0.0f, 0.1f, 0.0f };
			attack_item.delay = true;
			attack.attack_items.push_back(attack_item);
		}
		m_attack_config_presets.push_back(attack);
	}

	//m_pattern_index = 0;
	//m_attack_config = m_attack_config_presets[m_pattern_index];
}

void EnemyAttackPhasePatternAA::EnterAttackPrepare(float duration)
{
	m_player = GetOwner().FindGameObject<Player>();
	assert(!m_player.expired());

	ClearAttackPhaseObjects();
	DestroyItems();
	// create attack items
	const auto& item_configs = m_attack_config.attack_items;
	for (int i = 0; i < item_configs.size(); ++i)
	{
		auto item = GetOwner().CreateGameObject<EnemyAttackItem>();
		item->InitializeConfig(item_configs[i]);
		item->GetTransform().SetParent(&GetTransform());
		m_attack_phase_objects.push_back(item);
	}
	EnemyAttackPhasePatternBase::EnterAttackPrepare(duration);
}

void EnemyAttackPhasePatternAA::EnterAttackFollow(float duration)
{
	CreateItems();
	EnemyAttackPhasePatternBase::EnterAttackFollow(duration);
}

void EnemyAttackPhasePatternAA::EnterAttackPreExecute(float duration)
{
	EnemyAttackPhasePatternBase::EnterAttackPreExecute(duration);
}

void EnemyAttackPhasePatternAA::EnterAttackExecute(float duration)
{
	DestroyItems();
	EnemyAttackPhasePatternBase::EnterAttackExecute(duration);
}

void EnemyAttackPhasePatternAA::EnterAttackCooldown(float duration)
{
	
	EnemyAttackPhasePatternBase::EnterAttackCooldown(duration);
}

void EnemyAttackPhasePatternAA::ExitAttackCooldown()
{
	// DestroyItems();
	EnemyAttackPhasePatternBase::ExitAttackCooldown();
	//UpdatePatternIndex();
}

void EnemyAttackPhasePatternAA::UpdatePatternIndex()
{
	m_pattern_index = (m_pattern_index + 1) % m_attack_config_presets.size();
	m_attack_config = m_attack_config_presets[m_pattern_index];
}

void EnemyAttackPhasePatternAA::RestoreCheckPointStates()
{
	ClearAttackPhaseObjects();
	ClearItems();
}

void EnemyAttackPhasePatternAA::CreateItems()
{
	for (const auto& attack_config : m_attack_config.attack_items)
	{
		if (attack_config.delay)
		{
			continue;
		}

		auto item = GetOwner().CreateGameObject<DashItem>();
		DashItemConfig item_config{};
		item_config.position = attack_config.position;
		item->InitializeConfig(item_config);
		//item->GetTransform().SetParent(&m_room.lock()->GetTransform()); // TODO
		item->GetTransform().SetPosition(item_config.position);
		m_dash_items.push_back(item);
	}
}

void EnemyAttackPhasePatternAA::DestroyItems()
{
	for (auto& item_weak : m_dash_items)
	{
		if (auto item = item_weak.lock())
		{
			item->EnterDestroying();
		}
	}
	m_dash_items.clear();
}

void EnemyAttackPhasePatternAA::ClearItems()
{
	for (auto& item : m_dash_items)
	{
		if (auto ptr = item.lock())
		{
			ptr->EnterDestroying();
			ptr->SetObjectActive(false);
		}
	}
	m_dash_items.clear();
}
