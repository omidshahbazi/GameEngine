// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\RendererDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
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
			RendererDataManager::RendererDataManager(void) :
				m_IDAllocator("ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_MeshHandleAllocator("Mesh Handle Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(MeshFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_MaterialsAllocator("Materials Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(MaterialList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_IDs = IDFList(&m_IDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Meshes = MeshFList(&m_MeshHandleAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Materials = MaterialList(&m_MaterialsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			void RendererDataManager::Render(void)
			{
				static Matrix4F mat;
				mat.MakeIdentity();

				static DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				for (uint32 i = 0; i < m_IDs.GetSize(); ++i)
					device->DrawMesh(m_Meshes[i], mat, m_Materials[i]);
			}
		}
	}
}
