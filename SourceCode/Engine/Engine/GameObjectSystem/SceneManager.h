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
		using namespace Data;

		class GAMEOBJECTSYSTEM_API SceneManager
		{
			friend class Scene;

			SINGLETON_DECLARATION(SceneManager)

		private:
			SceneManager(void);
			~SceneManager(void);

		public:
			Scene CreateScene(void);

		private:
			SceneData *GetScene(IDType ID);

		private:
			SceneDataManager m_DataManager;
		};
	}
}

#endif