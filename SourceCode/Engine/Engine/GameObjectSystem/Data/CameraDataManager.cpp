// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\CameraDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <Containers\StringUtility.h>

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
				m_ViewProjectionMatricesAllocator("ViewProjection Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ColdDataAllocator("Camera ColdData Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ColdData) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ProjectionMatrices = Matrix4FList(&m_ProjectionMatricesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ViewProjectionMatrices = Matrix4FList(&m_ViewProjectionMatricesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ColdData = DataContainer<ColdData>(&m_ColdDataAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType CameraDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &projectionMat = m_ProjectionMatrices.Allocate();
				projectionMat.MakeIdentity();

				auto &viewProjectionMat = m_ViewProjectionMatrices.Allocate();
				viewProjectionMat.MakeIdentity();

				auto &coldData = m_ColdData.Allocate();
				coldData.Type = ProjectionTypes::Perspective;
				coldData.FieldOfView = 60;
				coldData.AspectRatio = 1;
				coldData.NearClipDistance = 1;
				coldData.FarClipDistance = 1000;

				UpdateProjectionMatrix(projectionMat, coldData);

				return id;
			}

			void CameraDataManager::SetFieldOfView(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.FieldOfView = Value;

				UpdateProjectionMatrix(m_ProjectionMatrices[index], coldData);
			}

			void CameraDataManager::SetAspectRatio(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.AspectRatio = Value;

				UpdateProjectionMatrix(m_ProjectionMatrices[index], coldData);

				Debugging::Debug::LogInfo(StringUtility::ToString<char8>(Value).GetValue());
			}

			void CameraDataManager::SetNearClipDistance(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.NearClipDistance = Value;

				UpdateProjectionMatrix(m_ProjectionMatrices[index], coldData);
			}

			void CameraDataManager::SetFarClipDistance(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.FarClipDistance = Value;

				UpdateProjectionMatrix(m_ProjectionMatrices[index], coldData);
			}

			void CameraDataManager::UpdateProjectionMatrix(Matrix4F & Matrix, const ColdData & ColdData)
			{
				Matrix.MakeIdentity();
				Matrix.MakePerspectiveProjectionMatrix(ColdData.FieldOfView, ColdData.AspectRatio, ColdData.NearClipDistance, ColdData.FarClipDistance);
			}

			void CameraDataManager::Update(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				Matrix4F *projectionMat = &m_ProjectionMatrices[0];
				Matrix4F *viewMat = GetSceneData()->Cameras.Transforms.m_WorldMatrices.GetData();
				Matrix4F *viewProjectionMat = &m_ViewProjectionMatrices[0];

				for (uint32 i = 0; i < size; ++i)
					viewProjectionMat[i] = projectionMat[i] * viewMat[i];
			}
		}
	}
}
