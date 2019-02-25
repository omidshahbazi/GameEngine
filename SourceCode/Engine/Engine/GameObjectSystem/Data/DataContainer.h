// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

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
			template<typename T>
			class DataContainer : public Vector<T>
			{
			public:
				DataContainer(void)
				{
				}

				DataContainer(AllocatorBase *Allocator, uint32 Capacity = 0) :
					Vector(Allocator, Capacity)
				{
				}

			public:
				T & Allocate(void)
				{
					uint32 index = Extend(1);

					return (*this)[index];
				}
			};

			typedef uint16 IDType;
			typedef DataContainer<Matrix4F> Matrix4FList;
		}
	}
}

#endif