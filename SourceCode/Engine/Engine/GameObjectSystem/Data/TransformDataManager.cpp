// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\TransformDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			TransformDataManager::TransformDataManager(SceneData *SceneData) :
				ComponentDataManager(SceneData),
				m_LocalMatrixAllocator("Local Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_WorldMatrixAllocator("World Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_LocalMatrices = Matrix4FList(&m_LocalMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_WorldMatrices = Matrix4FList(&m_WorldMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType TransformDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &localMat = m_LocalMatrices.Allocate();
				localMat.MakeIdentity();

				auto &worldMat = m_WorldMatrices.Allocate();
				worldMat.MakeIdentity();

				return id;
			}

			const Vector3F& TransformDataManager::GetForward(IDType ID)
			{
				int32 index = GetIndex(ID);

				return m_LocalMatrices[index].GetForward();
			}

			const Vector3F& TransformDataManager::GetLocalPosition(IDType ID)
			{
				int32 index = GetIndex(ID);

				return m_LocalMatrices[index].GetPosition();
			}

			void TransformDataManager::SetLocalPosition(IDType ID, const Vector3F & Value)
			{
				int32 index = GetIndex(ID);

				m_LocalMatrices[index].SetPosition(Value);
			}

			void TransformDataManager::SetLocalRotation(IDType ID, const Vector3F & Value)
			{
				int32 index = GetIndex(ID);

				m_LocalMatrices[index].SetRotation(Value);
			}

			void TransformDataManager::SetForward(IDType ID, const Vector3F & Value)
			{
				int32 index = GetIndex(ID);

				//m_LocalMatrices[index].set(Value);
			}

			void TransformDataManager::SetLocalScale(IDType ID, const Vector3F & Value)
			{
				int32 index = GetIndex(ID);

				m_LocalMatrices[index].SetScale(Value);
			}

			void TransformDataManager::Update(void)
			{
				static Matrix4F mat;
				mat.MakeIdentity();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				Matrix4F *localMat = &m_LocalMatrices[0];
				Matrix4F *worldMat = &m_WorldMatrices[0];

				for (uint32 i = 0; i < size; ++i)
					worldMat[i] = mat * localMat[i];
			}
		}
	}
}
