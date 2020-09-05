// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			TransformDataManager::TransformDataManager(SceneData* SceneData) :
				ComponentDataManager(SceneData),
				m_LocalMatrixAllocator("Local Matrix Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator),
				m_RotationMatrixAllocator("Rotation Matrix Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator),
				m_WorldMatrixAllocator("World Matrix Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator)
			{
				m_LocalMatrices = Matrix4FList(&m_LocalMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_LocalQuaternions = QuaternionFList(&m_RotationMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_WorldMatrices = Matrix4FList(&m_WorldMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType TransformDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto& localMat = m_LocalMatrices.Allocate();
				localMat = Matrix4F::Identity;

				auto& localQuat = m_LocalQuaternions.Allocate();
				localQuat = QuaternionF::Identity;

				auto& worldMat = m_WorldMatrices.Allocate();
				worldMat = Matrix4F::Identity;

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

				return m_LocalMatrices[index].GetTranslate();
			}

			void TransformDataManager::SetLocalPosition(IDType ID, const Vector3F& Value)
			{
				int32 index = GetIndex(ID);

				m_LocalMatrices[index].SetTranslate(Value);
			}

			void TransformDataManager::SetLocalRotation(IDType ID, const QuaternionF& Value)
			{
				int32 index = GetIndex(ID);

				m_LocalQuaternions[index] = Value;
			}

			void TransformDataManager::SetForward(IDType ID, const Vector3F& Value)
			{
				int32 index = GetIndex(ID);

				//m_LocalMatrices[index].set(Value);
			}

			void TransformDataManager::SetLocalScale(IDType ID, const Vector3F& Value)
			{
				int32 index = GetIndex(ID);

				m_LocalMatrices[index].SetScale(Value);
			}

			void TransformDataManager::Update(void)
			{
				static Matrix4F parentMat;
				parentMat = Matrix4F::Identity;

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				Matrix4F* localMat = &m_LocalMatrices[0];
				QuaternionF* localQuat = &m_LocalQuaternions[0];
				Matrix4F* worldMat = &m_WorldMatrices[0];

				Matrix4F rotMat;
				for (uint32 i = 0; i < size; ++i)
				{
					localQuat[i].ToMatrix(rotMat);

					worldMat[i] = parentMat * localMat[i] * rotMat;
				}
			}
		}
	}
}
