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
			SINGLETON_DECLARATION(SceneManager)

		private:
			SceneManager(void);
			~SceneManager(void);

		public:
			Scene CreateScene(void);

		private:
			Vector<SceneData> m_Scenes;

			SceneData::IDType m_LastID;
		};
	}
}

#endif