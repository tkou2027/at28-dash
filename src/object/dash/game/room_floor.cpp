#include "room_floor.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
// physics
#include "component/physics/component_collider.h"
// other objects
#include "object/game_object_list.h"
#include "object/dash/game/room.h"
#include "object/dash/game/block_visual.h"
#include "dash_item.h"

void RoomFloor::Initialize()
{
	m_components.Add<ComponentCollider>(m_comp_id_collider);
	m_components.Add<ComponentRendererMesh>(m_comp_id_mesh);
}

void RoomFloor::InitializeConfig(const FloorConfig& config, std::weak_ptr<Room> room)
{
	SetRoom(room);
	m_floor_config = config;
	m_transform.SetPosition(m_floor_config.position);
	InitializeVisuals();
	InitializeCollision();
}

void RoomFloor::InitializeVisuals()
{
	m_block_visuals.clear();
	int num_blocks = m_floor_config.block_num_x * m_floor_config.block_num_z;
	auto& comp_mesh = m_components.Get<ComponentRendererMesh>(m_comp_id_mesh);
	const Vector3 pos_offset{
		-((m_floor_config.block_num_x - 1) * 0.5f) * m_floor_config.block_scale,
		0.0f,
		-((m_floor_config.block_num_z - 1) * 0.5f) * m_floor_config.block_scale
	};
	for (int z = 0; z < m_floor_config.block_num_z; ++z)
	{
		for (int x = 0; x < m_floor_config.block_num_x; ++x)
		{
			auto block_visual = GetOwner().CreateGameObject<BlockVisual>();
			block_visual->InitializeConfig(); // TODO: type...
			m_block_visuals.push_back(block_visual);
			block_visual->GetTransform().SetParent(&m_transform);
			block_visual->GetTransform().SetPosition(
				Vector3{ x * m_floor_config.block_scale, 0.0f, z * m_floor_config.block_scale }
				+ pos_offset
			);
			block_visual->GetTransform().SetScale(m_floor_config.block_scale);
		}
	}
}

void RoomFloor::InitializeCollision()
{
	auto& comp_collider = m_components.Get<ComponentCollider>(m_comp_id_collider);

	float floor_width = m_floor_config.block_num_x * m_floor_config.block_scale;
	float floor_depth = m_floor_config.block_num_z * m_floor_config.block_scale;
	// ground
	{
		PolygonCylinder floor{};
		floor.sides = 4;
		floor.radius = Math::Max(floor_width, floor_depth) * Math::INV_SQRT_2; // TODO: config
		TransformNode3D transform{};
		transform.SetParent(&m_transform);
		transform.SetRotationYOnly(Math::PI * 0.25f);
		Collider border_collider{ &m_transform, ColliderLayer::Type::GROUND };
		border_collider.AddShape(ShapeCollider{ floor }, transform);
		border_collider.bounce = 0.0f;
		comp_collider.AddCollider(border_collider);
	}
	// border
	{
		for (int i = 0; i < 4; i++)
		{
			Plane border_plane{};
			TransformNode3D transform{};
			transform.SetParent(&m_transform);
			float rotation = Math::HALF_PI * i;
			Vector3 move_dir{ sinf(rotation), 0.0f, cosf(rotation) };
			transform.SetRotationYOnly(rotation + Math::PI);
			transform.SetPosition(move_dir * floor_width * 0.5f); // TODO: config
			Collider border_collider{ &m_transform, ColliderLayer::Type::BORDER };
			border_collider.AddShape(ShapeCollider{ border_plane }, transform);
			border_collider.bounce = 0.4f;
			comp_collider.AddCollider(border_collider);
		}
	}
}
