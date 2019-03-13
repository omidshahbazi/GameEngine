// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
			RendererDataManager::RendererDataManager(SceneData *SceneData) :
				ComponentDataManager(SceneData),
				m_MeshHandleAllocator("Mesh Handle Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(MeshFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_MaterialsAllocator("Materials Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(MaterialList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_Meshes = MeshFList(&m_MeshHandleAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Materials = MaterialList(&m_MaterialsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType RendererDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &mesh = m_Meshes.Allocate();
				mesh = nullptr;

				auto &material = m_Materials.Allocate();
				material = nullptr;

				return id;
			}

			void RendererDataManager::SetMesh(IDType ID, MeshHandle * Mesh)
			{
				int32 index = GetIndex(ID);

				m_Meshes[index] = Mesh;
			}

			void RendererDataManager::SetMaterial(IDType ID, Material * Material)
			{
				int32 index = GetIndex(ID);

				m_Materials[index] = Material;
			}

			void RendererDataManager::Render(void)
			{
				DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData *sceneData = GetSceneData();

				MeshHandle **mesh = m_Meshes.GetData();
				Material **material = m_Materials.GetData();
				Matrix4F *modelMat = sceneData->Renderables.Transforms.m_WorldMatrices.GetData();

				for (uint32 i = 0; i < size; ++i)
				{
					Matrix4F mvp = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[0] * modelMat[i];

					device->DrawMesh(mesh[i], mvp, material[i]);
				}
			}
		}
	}
}
