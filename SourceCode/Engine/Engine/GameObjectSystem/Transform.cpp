// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Transform.h>
#include <GameObjectSystem\SceneManager.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace GameObjectSystem
	{
		Transform::Transform(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		const Vector3F& Transform::GetForward(void)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				return sceneData->Cameras.Transforms.GetForward(m_ID);
			else if (m_Type == GameObjectTypes::Renderable)
				return sceneData->Renderables.Transforms.GetForward(m_ID);
			else if (m_Type == GameObjectTypes::Light)
				return sceneData->Lightings.Transforms.GetForward(m_ID);
			else if (m_Type == GameObjectTypes::TextRenderable)
				return sceneData->TextRenderables.Transforms.GetForward(m_ID);

			THROW_IF_EXCEPTION(Categories::GameObjectSystem, false, "m_Type is invalid");
		}

		const Vector3F& Transform::GetPosition(void)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				return sceneData->Cameras.Transforms.GetLocalPosition(m_ID);
			else if (m_Type == GameObjectTypes::Renderable)
				return sceneData->Renderables.Transforms.GetLocalPosition(m_ID);
			else if (m_Type == GameObjectTypes::Light)
				return sceneData->Lightings.Transforms.GetLocalPosition(m_ID);
			else if (m_Type == GameObjectTypes::TextRenderable)
				return sceneData->TextRenderables.Transforms.GetLocalPosition(m_ID);

			THROW_IF_EXCEPTION(Categories::GameObjectSystem, false, "m_Type is invalid");
		}

		void Transform::SetPosition(const Vector3F& Value)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalPosition(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalPosition(m_ID, Value);
			else if (m_Type == GameObjectTypes::Light)
				sceneData->Lightings.Transforms.SetLocalPosition(m_ID, Value);
			else if (m_Type == GameObjectTypes::TextRenderable)
				sceneData->TextRenderables.Transforms.SetLocalPosition(m_ID, Value);
		}

		void Transform::SetRotation(const Vector3F& Value)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			QuaternionF rot = QuaternionF::FromEuler(Value);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalRotation(m_ID, rot);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalRotation(m_ID, rot);
			else if (m_Type == GameObjectTypes::Light)
				sceneData->Lightings.Transforms.SetLocalRotation(m_ID, rot);
			else if (m_Type == GameObjectTypes::TextRenderable)
				sceneData->Lightings.Transforms.SetLocalRotation(m_ID, rot);
		}

		void Transform::SetRotation(const QuaternionF& Value)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalRotation(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalRotation(m_ID, Value);
			else if (m_Type == GameObjectTypes::Light)
				sceneData->Lightings.Transforms.SetLocalRotation(m_ID, Value);
			else if (m_Type == GameObjectTypes::TextRenderable)
				sceneData->Lightings.Transforms.SetLocalRotation(m_ID, Value);
		}

		void Transform::SetForward(const Vector3F& Value)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetForward(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetForward(m_ID, Value);
			else if (m_Type == GameObjectTypes::Light)
				sceneData->Lightings.Transforms.SetForward(m_ID, Value);
			else if (m_Type == GameObjectTypes::TextRenderable)
				sceneData->Lightings.Transforms.SetForward(m_ID, Value);
		}

		void Transform::SetScale(const Vector3F& Value)
		{
			SceneData* sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			if (m_Type == GameObjectTypes::Camera)
				sceneData->Cameras.Transforms.SetLocalScale(m_ID, Value);
			else if (m_Type == GameObjectTypes::Renderable)
				sceneData->Renderables.Transforms.SetLocalScale(m_ID, Value);
			else if (m_Type == GameObjectTypes::Light)
				sceneData->Lightings.Transforms.SetLocalScale(m_ID, Value);
			else if (m_Type == GameObjectTypes::TextRenderable)
				sceneData->Lightings.Transforms.SetLocalScale(m_ID, Value);
		}
	}
}