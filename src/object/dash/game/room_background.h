#pragma once
#include "object/game_object.h"

class RoomBackground : public GameObject
{
public:
	int GetOrCreateMaterialDescDefault() const;
	int GetOrCreateMaterialDescMask() const;
	void Initialize() override;
	void InitializeConfig(const std::string& key);
private:
	ComponentId m_comp_id_mesh{};
};