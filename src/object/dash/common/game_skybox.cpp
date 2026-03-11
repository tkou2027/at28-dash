#include "game_skybox.h"
#include "object/game_object_list.h"
#include "object/common/skybox.h"

#include "global_context.h"
#include "config/preset_manager.h"

void GameSkybox::Initialize()
{
	{
		auto skybox_obj_a = GetOwner().CreateGameObject<Skybox>();
		const auto& texture_id = g_global_context.m_preset_manager->GetTextureId("texture/skybox/default");
		skybox_obj_a->SetTexture(texture_id);
		skybox_obj_a->SetStencilRef(0);
	}
	{
		auto skybox_obj_b = GetOwner().CreateGameObject<Skybox>();
		const auto& texture_id = g_global_context.m_preset_manager->GetTextureId("texture/skybox/mask");
		skybox_obj_b->SetTexture(texture_id);
		skybox_obj_b->SetStencilRef(1);
		skybox_obj_b->SetFluid(true);
	}
}
