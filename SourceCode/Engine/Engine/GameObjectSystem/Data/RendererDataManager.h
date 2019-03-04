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

				void Render(void);

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