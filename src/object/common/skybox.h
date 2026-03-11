#pragma once
#include "object/game_object.h"
#include "render/config/texture_resource_id.h"

class Skybox : public GameObject
{
public:
	void Initialize() override;
	void SetTexture(TextureResourceId texture_id);
	void SetStencilRef(int stencil_ref);
	void SetFluid(bool fliud);
private:
	ComponentId m_comp_id_render;
};