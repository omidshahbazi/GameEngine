// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\GameObject.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		GameObject::GameObject(void) :
			m_SceneID(0),
			m_ID(0),
			m_Type(GameObjectTypes::None)
		{
		}

		GameObject::GameObject(IDType SceneID, IDType ID, GameObjectTypes Type) :
			m_SceneID(SceneID),
			m_ID(ID),
			m_Type(Type)
		{
		}

		Transform GameObject::GetTransform(void)
		{
			return Transform(m_SceneID, m_ID, m_Type);
		}

		Camera GameObject::GetCamera(void)
		{
			return Camera(m_SceneID, m_ID, m_Type);
		}

		Renderer GameObject::GetRenderer(void)
		{
			return Renderer(m_SceneID, m_ID, m_Type);
		}
	}
}