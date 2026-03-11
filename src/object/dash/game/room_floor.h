#pragma once
#include "object/dash/game/room_object.h"
#include "object/dash/game/game_util/field_config.h"

class RoomFloor : public RoomObject
{
public:
	void Initialize() override;
	void InitializeConfig(const FloorConfig& config, std::weak_ptr<Room> room);
private:
	void InitializeVisuals();
	void InitializeCollision();
	// parts
	ComponentId m_comp_id_collider{};
	ComponentId m_comp_id_mesh{};
	FloorConfig m_floor_config{};
	std::vector<std::weak_ptr<class BlockVisual> > m_block_visuals{};
};