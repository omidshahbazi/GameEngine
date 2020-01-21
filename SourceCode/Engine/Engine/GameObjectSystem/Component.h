// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

#include <GameObjectSystem\Data\DataContainer.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Component
		{
		public:
			Component(IDType SceneID, IDType ID, GameObjectTypes Type) :
				m_SceneID(SceneID),
				m_ID(ID),
				m_Type(Type)
			{
			}

		protected:
			IDType m_SceneID;
			IDType m_ID;
			GameObjectTypes m_Type;
		};
	}
}

#endif