#include "field_config.h"

namespace
{
	static EnemyPrimeConfig CreateEnemyPrime()
	{
		EnemyPrimeConfig enemy{};
		// attack phase 0
		{
			EnemyAttackPhaseConfig attack_phase{};
			// attack 0 - item attack
			{
				EnemyAttackConfig attack{};

				attack.type = EnemyAttackType::ITEM;
				DashItemConfig item{};
				float size = 20.0f;
				item.position = Vector3{ size, 0.0f, size };
				attack.items.push_back(item);
				item.position = Vector3{ -size, 0.0f, size };
				attack.items.push_back(item);
				item.position = Vector3{ size, 0.0f, -size };	
				attack.items.push_back(item);
				item.position = Vector3{ -size, 0.0f, -size };
				attack.items.push_back(item);
				
				EnemyAttackItemConfig attack_item{};
				attack_item.position = Vector3{ size, 0.0f, size };
				attack.attack_items.push_back(attack_item);
				attack_item.position = Vector3{ -size, 0.0f, size };
				attack.attack_items.push_back(attack_item);
				attack_item.position = Vector3{ size, 0.0f, -size };
				attack.attack_items.push_back(attack_item);
				attack_item.position = Vector3{ -size, 0.0f, -size };
				attack.attack_items.push_back(attack_item);


				attack_phase.attacks.push_back(attack);
			}
			enemy.attack_phases.push_back(attack_phase);
		}
		return enemy;
	}

	static RoomConfig CreateRoom0()
	{
		RoomConfig room{};
		{
			FloorConfig floor{};
			floor.block_num_x = 8;
			floor.block_num_z = 8;
			floor.block_scale = 10.0f;
			room.floors.push_back(floor);
		}
		// enemy waves
		{
			// wave 0 - enemy prime
			{
				EnemyWaveConfig wave{};
				// enemy 0
				EnemyPrimeConfig enemy = CreateEnemyPrime();
				enemy.position = Vector3{ 0.0f, 0.0f, 60.0f };
				wave.m_enemies_prime.push_back(enemy);

				// wave
				room.enemy_waves.push_back(wave);
			}
		}
		// items
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ 30.0f, 0.0f, 0.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ -30.0f, 0.0f, 0.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ 10.0f, 0.0f, 20.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ -10.0f, 0.0f, 20.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ 20.0f, 0.0f, 10.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ -20.0f, 0.0f, 10.0f };
		//	room.dash_items.push_back(dash_item);
		//}
		//{
		//	DashItemConfig dash_item{};
		//	dash_item.position = Vector3{ 0.0f, 0.0f, 60.0f };
		//	dash_item.height = 30.0f;
		//	dash_item.radius = dash_item.height * Math::INV_SQRT_2;
		//	dash_item.hp_max = 20.0f;
		//	room.dash_items.push_back(dash_item);
		//}
		return room;
	}
} // namespace

FieldConfig g_field_config
{
	{
		CreateRoom0()
	}
};