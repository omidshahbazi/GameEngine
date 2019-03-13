// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <GameObjectSystem\Data\SceneDataManager.h>
#include <GameObjectSystem\Scene.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		class GameObject;
		class Transform;
		class Camera;
		class Renderer;

		using namespace Data;

		class GAMEOBJECTSYSTEM_API SceneManager
		{
			friend class Scene;
			friend class GameObject;
			friend class Transform;
			friend class Camera;
			friend class Renderer;

			SINGLETON_DECLARATION(SceneManager)

		private:
			SceneManager(void);

		public:
			Scene CreateScene(void);

			Scene GetActiveScene(void) const;
			void SetActiveScene(Scene Scene)
			{
				m_ActiveScene = Scene.m_ID;
			}

		private:
			SceneData *GetSceneData(IDType ID);

		private:
			SceneDataManager m_DataManager;

			IDType m_ActiveScene;
		};
	}
}

#endif