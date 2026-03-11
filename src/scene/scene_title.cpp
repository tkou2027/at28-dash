#include "scene_title.h"
#include "object/dash/title/title_manager.h"

void SceneTitle::Initialize()
{
	m_objects.CreateGameObject<TitleManager>();
}
