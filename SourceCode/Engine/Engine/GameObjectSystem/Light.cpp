// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Light.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Light::Light(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		void Light::SetType(LightTypes Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetType(m_ID, Value);
		}

		void Light::SetColor(Color Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetColor(m_ID, Value);
		}

		void Light::SetStrength(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetStrength(m_ID, Value);
		}

		void Light::SetRadius(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetRadius(m_ID, Value);
		}

		void Light::SetConstantAttenuation(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetConstantAttenuation(m_ID, Value);
		}

		void Light::SetLinearAttenuation(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetLinearAttenuation(m_ID, Value);
		}

		void Light::SetQuadraticAttenuation(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetQuadraticAttenuation(m_ID, Value);
		}

		void Light::SetInnerCutOff(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetInnerCutOff(m_ID, Value);
		}

		void Light::SetOuterCutOff(float32 Value)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Lightings.Lights.SetOuterCutOff(m_ID, Value);
		}
	}
}