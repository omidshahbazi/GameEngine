// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		template<typename T>
		class Vector
		{
		public:
			typedef T ItemType;

			void Add(const T &Item)
			{

			}

		private:
			uint32 m_Capacity;
			uint32 m_Size;
			T *m_Items;
			AllocatorBase *m_Allocator;
		};
	}
}

#endif