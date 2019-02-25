// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\GameObject.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		GameObject::GameObject(IDType SceneID, IDType GameObjectID) :
			m_SceneID(SceneID),
			m_GameObjectID(GameObjectID)
		{
		}
	}
}