// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_DATA_MANAGER_H
#define RENDERER_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API RendererDataManager : public ComponentDataManager
			{
			public:
				RendererDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetMesh(IDType ID, MeshHandle *Mesh);
				void SetMaterial(IDType ID, Material *Material);

				void Update(void) override
				{
				}

				void Render(void) override;

			private:
				MeshFList m_Meshes;
				MaterialList m_Materials;

				DynamicSizeAllocator m_MeshHandleAllocator;
				DynamicSizeAllocator m_MaterialsAllocator;
			};
		}
	}
}

#endif