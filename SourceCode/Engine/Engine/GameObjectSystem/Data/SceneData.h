// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <GameObjectSystem\Data\GameObjectDataManager.h>
#include <GameObjectSystem\Data\TransformDataManager.h>
#include <GameObjectSystem\Data\CameraDataManager.h>
#include <GameObjectSystem\Data\RendererDataManager.h>
#include <GameObjectSystem\Data\LighDataManager.h>

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
				struct CamerasObjects
				{
				public:
					CamerasObjects(SceneData *SceneData) :
						GameObjects(SceneData),
						Transforms(SceneData),
						Cameras(SceneData)
					{
					}

					GameObjectDataManager GameObjects;
					TransformDataManager Transforms;
					CameraDataManager Cameras;
				};

				struct RenderableObjects
				{
				public:
					RenderableObjects(SceneData *SceneData) :
						GameObjects(SceneData),
						Transforms(SceneData),
						Renderers(SceneData)
					{
					}

					GameObjectDataManager GameObjects;
					TransformDataManager Transforms;
					RendererDataManager Renderers;
				};

				struct LightinngObjects
				{
				public:
					LightinngObjects(SceneData *SceneData) :
						GameObjects(SceneData),
						Transforms(SceneData),
						Lights(SceneData)
					{
					}

					GameObjectDataManager GameObjects;
					TransformDataManager Transforms;
					LighDataManager Lights;
				};

			public:
				SceneData(IDType ID) :
					ID(ID),
					Cameras(this),
					Renderables(this),
					Lightings(this)
				{
				}

			public:
				IDType ID;

				CamerasObjects Cameras;
				RenderableObjects Renderables;
				LightinngObjects Lightings;
			};
		}
	}
}

#endif