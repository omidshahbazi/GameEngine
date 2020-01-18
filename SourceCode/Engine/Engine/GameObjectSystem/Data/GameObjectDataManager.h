// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_DATA_MANAGER_H
#define GAME_OBJECT_DATA_MANAGER_H

#include <GameObjectSystem\Data\DataManagerBase.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		class GameObject;

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API GameObjectDataManager : public DataManagerBase
			{
				friend class GameObject;

			public:
				GameObjectDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void Update(void) override
				{
				}
				void Render(void) override
				{
				}

			private:
				IDFList m_ParentIDs;

				DynamicSizeAllocator m_ParentIDAllocator;
			};
		}
	}
}

#endif