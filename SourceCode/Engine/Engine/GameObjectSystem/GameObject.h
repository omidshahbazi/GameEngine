// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API GameObject
		{
		public:
			GameObject(IDType SceneID, IDType GameObjectID);

		private:
			IDType m_SceneID;
			IDType m_GameObjectID;
		};
	}
}

#endif