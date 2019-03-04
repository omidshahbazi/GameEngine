// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <GameObjectSystem\Data\GameObjectDataManager.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		class Scene;
		class GameObject;

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API SceneData
			{
				friend class Scene;
				friend class GameObject;

			public:
				SceneData(void);

			public:
				IDType ID;

				GameObjectDataManager GameObjects;
			};
		}
	}
}

#endif