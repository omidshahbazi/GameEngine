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
				GameObjectDataManager(void);

				IDType Create(void) override;

				void Update(void) override;
				void Render(void) override
				{
				}

			private:
				IDFList m_ParentIDs;
				Matrix4FList m_LocalMatrices;
				Matrix4FList m_WorldMatrices;
				ComponentMaskList m_ComponentMasks;

				DynamicSizeAllocator m_ParentIDAllocator;
				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
				DynamicSizeAllocator m_ComponentMaskAllocator;
			};
		}
	}
}

#endif