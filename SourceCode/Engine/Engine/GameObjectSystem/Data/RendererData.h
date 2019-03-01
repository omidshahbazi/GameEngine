// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_DATA_H
#define RENDERER_DATA_H

#include <GameObjectSystem\Data\DataContainer.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		class Scene;
		class GameObject;

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API RendererData
			{
				friend class Scene;
				friend class GameObject;

			public:
				RendererData(void);

			public:
				IDType ID;

				IDFList IDs;
				IDFList ParentIDs;
				Matrix4FList LocalMatrices;
				Matrix4FList WorldMatrices;
				ComponentMaskList ComponentMasks;

			private:
				SceneData::IDType m_LastGameObjectID;

				DynamicSizeAllocator m_IDAllocator;
				DynamicSizeAllocator m_ParentIDAllocator;
				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
				DynamicSizeAllocator m_ComponentMaskAllocator;
			};
		}
	}
}

#endif