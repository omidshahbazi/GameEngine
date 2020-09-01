// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\DataManagerBase.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			DataManagerBase::DataManagerBase(SceneData *SceneData) :
				m_SceneData(SceneData),
				m_IDAllocator("ID Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_IDs = IDFList(&m_IDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType DataManagerBase::Create(void)
			{
				++m_LastID;

				auto &id = m_IDs.Allocate();
				id = m_LastID;

				return m_LastID;
			}

			int32 DataManagerBase::GetIndex(IDType ID) const
			{
				for (uint32 i = 0; i < m_IDs.GetSize(); ++i)
					if (m_IDs[i] == ID)
						return i;

				return -1;
			}
		}
	}
}
