#pragma once
#include "object/game_object.h"

class BlockVisual : public GameObject
{
public:
	int GetOrCreateMaterialDescDefault() const;
	int GetOrCreateMaterialDescMask() const;
	void Initialize() override;
	void InitializeConfig(); // TODO: type...
	ComponentId m_comp_id_mesh{};
};