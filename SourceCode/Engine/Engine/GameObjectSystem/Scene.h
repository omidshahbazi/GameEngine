// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <GameObjectSystem\Data\SceneData.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Scene
		{
		public:
			Scene(SceneData::IDType ID) :
				m_ID(ID)
			{
			}

		private:
			SceneData::IDType m_ID;
		};
	}
}

#endif