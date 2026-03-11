#include "room_object.h"
#include "room.h"

void RoomObject::SetRoom(std::weak_ptr<Room> room)
{
	m_room = room;
	m_transform.SetParent(&m_room.lock()->GetTransform());
}

void RoomObject::Update()
{
	Room::RoomState m_state = m_room.lock()->GetState();
	switch (m_state)
	{
	case Room::RoomState::TRANS_IN_STAGE_1:
	{
		UpdateRoomStateTransInFirst();
		break;
	}
	case Room::RoomState::TRANS_IN_STAGE_2:
	{
		UpdateRoomStateTransInSecond();
		break;
	}
	case Room::RoomState::ACTIVE:
	{
		UpdateRoomStateActive();
		break;
	}
	case Room::RoomState::TRANS_OUT_STAGE_1:
	{
		UpdateRoomStateTransOutFirst();
		break;
	}
	case Room::RoomState::TRANS_OUT_STAGE_2:
	{
		UpdateRoomStateTransOutSecond();
		break;
	}
	}
}

const RoomConfig& RoomObject::GetRoomConfig() const
{
	return m_room.lock()->GetConfig();
}
