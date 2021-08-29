// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Camera.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Camera::Camera(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		void Camera::SetFieldOfView(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Cameras.Cameras.SetFieldOfView(m_ID, Value);
		}

		void Camera::SetAspectRatio(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Cameras.Cameras.SetAspectRatio(m_ID, Value);
		}

		void Camera::SetNearClipDistance(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Cameras.Cameras.SetNearClipDistance(m_ID, Value);
		}

		void Camera::SetFarClipDistance(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Cameras.Cameras.SetFarClipDistance(m_ID, Value);
		}
	}
}