// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <MemoryManagement\Singleton.h>
#include <Containers\Vector.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\Mesh.h>
#include <Rendering\Material.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace GameObjectSystem
	{
		namespace Data
		{
			template<typename T>
			class DataContainer : public Vector<T>
			{
			public:
				DataContainer(void)
				{
				}

				DataContainer(AllocatorBase* Allocator, uint32 Capacity = 0) :
					Vector(Allocator, Capacity)
				{
				}

				virtual ~DataContainer(void)
				{
				}

			public:
				T& Allocate(void)
				{
					uint32 index = Extend(1);

					return (*this)[index];
				}
			};

			typedef uint16 IDType;
			typedef uint64 ComponentMask;
			typedef DataContainer<IDType> IDFList;
			typedef DataContainer<ComponentMask> ComponentMaskList;
			typedef DataContainer<Matrix4F> Matrix4FList;
			typedef DataContainer<QuaternionF> QuaternionFList;
			typedef DataContainer<MeshHandle*> MeshFList;
			typedef DataContainer<Material*> MaterialList;
		}
	}
}

#endif