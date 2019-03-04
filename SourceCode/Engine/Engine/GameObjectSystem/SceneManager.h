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
		class Renderer;

		using namespace Data;

		class GAMEOBJECTSYSTEM_API SceneManager
		{
			friend class Scene;
			friend class GameObject;
			friend class Renderer;

			SINGLETON_DECLARATION(SceneManager)

		private:
			SceneManager(void);

		public:
			Scene CreateScene(void);

		private:
			SceneData *GetSceneData(IDType ID);

		private:
			SceneDataManager m_DataManager;
		};
	}
}

#endif