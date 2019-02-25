// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <GameObjectSystem\Scene.h>
#include <MemoryManagement\Singleton.h>

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
			DataContainer<SceneData> m_Scenes;

			IDType m_LastID;
		};
	}
}

#endif