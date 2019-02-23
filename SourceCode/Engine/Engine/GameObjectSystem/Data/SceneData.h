// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <MemoryManagement\Singleton.h>
#include <Containers\Vector.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API SceneData
			{
			public:
				typedef uint16 IDType;
				typedef Vector<Matrix4F> Matrix4FList;

			public:
				SceneData(void);

			public:
				IDType ID;
				Matrix4FList LocalMatrices;
				Matrix4FList WorldMatrices;

			private:
				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
			};
		}
	}
}

#endif