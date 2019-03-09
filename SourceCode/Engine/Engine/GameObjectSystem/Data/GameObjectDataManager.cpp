// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\GameObjectDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			GameObjectDataManager::GameObjectDataManager(void) :
				m_ParentIDAllocator("Parent ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ComponentMaskAllocator("Component Mask Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ComponentMaskList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ParentIDs = IDFList(&m_ParentIDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ComponentMasks = ComponentMaskList(&m_ComponentMaskAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType GameObjectDataManager::Create(void)
			{
				auto id = DataManagerBase::Create();

				auto &parentID = m_ParentIDs.Allocate();
				parentID = -1;

				auto &comMask = m_ComponentMasks.Allocate();
				comMask = 0;

				return id;
			}

			void GameObjectDataManager::SetComponentFlagState(IDType ID, ComponentTypes Component, bool Enabled)
			{
				int32 index = GetIndex(ID);

				auto &mask = m_ComponentMasks[index];

				if (Enabled)
					BitwiseUtils::Enable(mask, (ComponentMask)Component);
				else
					BitwiseUtils::Disable(mask, (ComponentMask)Component);
			}
		}
	}
}
