// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <GameObjectSystem\Data\GameObjectDataManager.h>
#include <GameObjectSystem\Data\TransformDataManager.h>
#include <GameObjectSystem\Data\CameraDataManager.h>
#include <GameObjectSystem\Data\RendererDataManager.h>

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
				SceneData(IDType ID) :
					ID(ID),
					GameObjects(this),
					Transforms(this),
					Cameras(this),
					Renderers(this)
				{
				}


			public:
				IDType ID;

				GameObjectDataManager GameObjects;
				TransformDataManager Transforms;
				CameraDataManager Cameras;
				RendererDataManager Renderers;
			};
		}
	}
}

#endif