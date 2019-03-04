// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_DATA_MANAGER_H
#define RENDERER_DATA_MANAGER_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API RendererDataManager
			{
			public:
				RendererDataManager(void);

				IDType Create(void);

				void SetMesh(IDType ID, MeshHandle *Mesh);
				void SetMaterial(IDType ID, Material *Material);

				void Render(void);

			protected:
				int32 GetIndex(IDType ID) const;

			private:
				IDType m_LastID;

				IDFList m_IDs;
				MeshFList m_Meshes;
				MaterialList m_Materials;

				DynamicSizeAllocator m_IDAllocator;
				DynamicSizeAllocator m_MeshHandleAllocator;
				DynamicSizeAllocator m_MaterialsAllocator;
			};
		}
	}
}

#endif