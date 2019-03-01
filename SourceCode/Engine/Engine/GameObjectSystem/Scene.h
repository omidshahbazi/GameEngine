// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <GameObjectSystem\GameObject.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class SceneData;
		}

		using namespace Data;

		class GAMEOBJECTSYSTEM_API Scene
		{
		public:
			Scene(IDType ID);

			GameObject CreateGameObject(void);

			void Update(void);

		private:
			void UpdateWorldMatrices(const Matrix4F &ViewProjection);

			SceneData * GetScene(void);

		private:
			IDType m_ID;
		};
	}
}

#endif