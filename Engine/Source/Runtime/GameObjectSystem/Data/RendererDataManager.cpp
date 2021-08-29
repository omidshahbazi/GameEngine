// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\RendererDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			RendererDataManager::RendererDataManager(SceneData* SceneData) :
				ComponentDataManager(SceneData),
				m_MeshHandlesAllocator("Mesh Handles Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator),
				m_MaterialsAllocator("Materials Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator)
			{
				m_Meshes = MeshList(&m_MeshHandlesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Materials = MaterialList(&m_MaterialsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType RendererDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto& mesh = m_Meshes.Allocate();
				mesh = nullptr;

				auto& material = m_Materials.Allocate();
				material = nullptr;

				return id;
			}

			void RendererDataManager::SetMesh(IDType ID, MeshResource* Mesh)
			{
				int32 index = GetIndex(ID);

				m_Meshes[index] = Mesh;
			}

			void RendererDataManager::SetMaterial(IDType ID, MaterialResource* Material)
			{
				int32 index = GetIndex(ID);

				m_Materials[index] = Material;
			}

			void RendererDataManager::Render(void)
			{
				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData* sceneData = GetSceneData();

				MeshResource** mesh = m_Meshes.GetData();
				MaterialResource** material = m_Materials.GetData();
				Matrix4F* modelMat = sceneData->Renderables.Transforms.m_WorldMatrices.GetData();

				int32 cameraIndex = 0;
				const Matrix4F& view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];
				const Matrix4F& projection = sceneData->Cameras.Cameras.m_ProjectionMatrices[cameraIndex];
				const Matrix4F& viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				for (uint32 i = 0; i < size; ++i)
				{
					Matrix4F mvp = viewProjection * modelMat[i];

					device->DrawMesh(**mesh[i], modelMat[i], view, projection, mvp, **material[i]);
				}
			}
		}
	}
}
