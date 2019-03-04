// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_DATA_MANAGER_H
#define SCENE_DATA_MANAGER_H

#include <GameObjectSystem\Data\SceneData.h>
#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API SceneDataManager
			{
			public:
				SceneDataManager(void);

			public:
				SceneData * Create(void);

				SceneData *GetScene(IDType ID);

			private:
				IDType m_LastID;

				DataContainer<SceneData> m_Scenes;
			};
		}
	}
}

#endif