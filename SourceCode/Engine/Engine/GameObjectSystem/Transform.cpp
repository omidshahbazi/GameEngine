// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Transform.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Transform::Transform(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		void Transform::SetPosition(const Vector3F & Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalPosition(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalPosition(m_ID, Value);
		}

		void Transform::SetRotation(const Vector3F & Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalRotation(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalRotation(m_ID, Value);
		}

		void Transform::SetScale(const Vector3F & Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalScale(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalScale(m_ID, Value);
		}
	}
}