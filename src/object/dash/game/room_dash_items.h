#pragma once
#include "object/dash/game/room_object.h"
#include "object/dash/game/game_util/field_config.h"

class RoomDashItems : public RoomObject
{
public:
	void Initialize() override;
	void InitializeConfig(std::weak_ptr<Room> room);
private:
	void InitializeItems();
	// parts
	std::vector<std::weak_ptr<class DashItem> > m_dash_items{};
};