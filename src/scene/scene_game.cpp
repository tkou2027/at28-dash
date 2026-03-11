#include "scene_game.h"
#include "object/dash/game/game_manager.h"

void SceneGame::Initialize()
{
	m_objects.CreateGameObject<GameManager>();
}
