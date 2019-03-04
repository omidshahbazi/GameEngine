// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_DATA_MANAGER_H
#define GAME_OBJECT_DATA_MANAGER_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		class GameObject;

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API GameObjectDataManager
			{
				friend class GameObject;

			public:
				GameObjectDataManager(void);

				IDType CreateGameObject(void);

				void UpdateWorldMatrices(const Matrix4F &ViewProjection);

			private:
				IDType m_LastID;

				IDFList m_IDs;
				IDFList m_ParentIDs;
				Matrix4FList m_LocalMatrices;
				Matrix4FList m_WorldMatrices;
				ComponentMaskList m_ComponentMasks;

				DynamicSizeAllocator m_IDAllocator;
				DynamicSizeAllocator m_ParentIDAllocator;
				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
				DynamicSizeAllocator m_ComponentMaskAllocator;
			};
		}
	}
}

#endif