#pragma once
#include "object/game_object.h"
#include "game_util/field_config.h"
class Room;
class RoomObject : public GameObject
{
public:
	void SetRoom(std::weak_ptr<Room> room);
	virtual void Update() override;
	virtual void EnterRoomStateIdle() {};
	virtual void EnterRoomStateTransInFirst(float duration) {};
	virtual void UpdateRoomStateTransInFirst() {};
	virtual void EnterRoomStateTransInSecond(float duration) {};
	virtual void UpdateRoomStateTransInSecond() {};
	virtual void EnterRoomStateActive() {};
	virtual void UpdateRoomStateActive() {};
	virtual void EnterRoomStateTransOut(float duration) {};
	virtual void UpdateRoomStateTransOutFirst() {};
	virtual void EnterRoomStateTransOutSecond(float duration) {};
	virtual void UpdateRoomStateTransOutSecond() {};
	virtual void EnterRoomStateDone() {};
protected:
	const RoomConfig& GetRoomConfig() const;
private:
	std::weak_ptr<Room> m_room;
};