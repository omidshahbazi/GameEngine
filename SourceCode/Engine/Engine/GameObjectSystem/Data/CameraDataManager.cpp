// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\CameraDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			CameraDataManager::CameraDataManager(SceneData *SceneData) :
				ComponentDataManager(SceneData),
				m_ProjectionMatricesAllocator("View Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ViewProjectionMatricesAllocator("ViewProjection Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ProjectionMatrices = Matrix4FList(&m_ProjectionMatricesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ViewProjectionMatrices = Matrix4FList(&m_ViewProjectionMatricesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType CameraDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &localMat = m_ProjectionMatrices.Allocate();
				localMat.MakeIdentity();

				return id;
			}

			void CameraDataManager::Update(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				IDType *goIDs = &m_GameObjectIDs[0];
				Matrix4F *projectionMat = &m_ProjectionMatrices[0];
				Matrix4F *viewProjectionMat = &m_ViewProjectionMatrices[0];

				TransformDataManager &transformDataManager = GetSceneData()->Transforms;

				for (uint32 i = 0; i < size; ++i)
					viewProjectionMat[i] = projectionMat[i] * transformDataManager.GetWorldMatrixByGameObjectID(goIDs[i]);
			}
		}
	}
}
