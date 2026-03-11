#pragma once
#include "object/game_object.h"
#include <deque>

class DashItem;
class Field;
class DashController : public GameObject
{
public:
	void Initialize() override;
	void SetField(std::weak_ptr<Field> field);
	void AddDashItem(std::weak_ptr<DashItem> dash_item);
	void RemoveDashItem(std::weak_ptr<DashItem> dash_item);
	bool IfCanDash() const;
	Vector3 GetAndAssureDashTarget() const;
	void TriggerDashItem();
	void ReleaseDashItem();
	void Reset();
	const std::deque<std::weak_ptr<DashItem> >& GetDashItems() const { return m_dash_items; }
private:
	std::deque<std::weak_ptr<DashItem> > m_dash_items;
	std::weak_ptr<DashItem> m_dash_item_active;
	std::weak_ptr<Field> m_field;
};