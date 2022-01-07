// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_DATA_MANAGER_H
#define RENDERER_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <RenderSystem\CommandBuffer.h>

namespace Engine
{
	using namespace RenderSystem;

	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API RendererDataManager : public ComponentDataManager
			{
			public:
				RendererDataManager(SceneData* SceneData);

				IDType Create(void) override;

				void SetMesh(IDType ID, MeshResource* Mesh);
				void SetMaterial(IDType ID, MaterialResource* Material);

				void Update(void) override
				{
				}

				void Render(void) override;

			private:
				MeshList m_Meshes;
				MaterialList m_Materials;

				DynamicSizeAllocator m_MeshHandlesAllocator;
				DynamicSizeAllocator m_MaterialsAllocator;

				CommandBuffer m_CommandBuffer;
			};
		}
	}
}

#endif