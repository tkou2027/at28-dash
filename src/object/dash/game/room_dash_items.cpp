#include "room_dash_items.h"
// render
#include "component/render/component_renderer_mesh.h"
#include "config/preset_manager.h"
#include "render/render_resource.h"

#include "object/game_object_list.h"
#include "object/dash/game/room.h"
#include "object/dash/game/dash_item.h"

void RoomDashItems::Initialize()
{
	
}

void RoomDashItems::InitializeConfig(std::weak_ptr<Room> room)
{
	SetRoom(room);
	m_dash_items.clear();
	const auto& room_config = GetRoomConfig();
	for (const auto& dash_item_config : room_config.dash_items)
	{
		auto dash_item = GetOwner().CreateGameObject<DashItem>();
		dash_item->InitializeConfig(dash_item_config);
		m_dash_items.push_back(dash_item);
		dash_item->GetTransform().SetParent(&m_transform);
		dash_item->GetTransform().SetPosition(dash_item_config.position);
	}
}
