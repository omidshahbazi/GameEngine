// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\CameraDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			CameraDataManager::CameraDataManager(void) :
				m_ViewMatricesAllocator("View Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ViewMatrices = Matrix4FList(&m_ViewMatricesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType CameraDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &localMat = m_ViewMatrices.Allocate();
				localMat.MakeIdentity();

				return id;
			}

			void CameraDataManager::Update(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				Matrix4F *viewMat = &m_ViewMatrices[0];

				//for (uint32 i = 0; i < size; ++i)
				//	worldMat[i] = mat * localMat[i];
			}
		}
	}
}
