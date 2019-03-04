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

			IDType RendererDataManager::Create(void)
			{
				++m_LastID;

				auto &id = m_IDs.Allocate();
				id = m_LastID;

				auto &mesh = m_Meshes.Allocate();
				mesh = nullptr;

				auto &material = m_Materials.Allocate();
				material = nullptr;

				return m_LastID;
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
				static Matrix4F mat;
				mat.MakeIdentity();

				static DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				MeshHandle **mesh = &m_Meshes[0];
				Material **material = &m_Materials[0];

				for (uint32 i = 0; i < size; ++i)
					device->DrawMesh(mesh[i], mat, material[i]);
			}

			int32 RendererDataManager::GetIndex(IDType ID) const
			{
				for (uint32 i = 0; i < m_IDs.GetSize(); ++i)
					if (m_IDs[i] == ID)
						return i;

				return -1;
			}
		}
	}
}
