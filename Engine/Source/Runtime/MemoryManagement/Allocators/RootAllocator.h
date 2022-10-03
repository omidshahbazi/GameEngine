// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ROOT_ALLOCATOR_H
#define ROOT_ALLOCATOR_H

#include <Allocators\DynamicSizeAllocator.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace MemoryManagement;

	namespace Allocators
	{
		class ALLOCATORS_API RootAllocator : public DynamicSizeAllocator
		{
			SINGLETON_DECLARATION(RootAllocator);

		private:
			static const uint64 ROOT_ALLOCATOR_DEFAULT_RESERVE_SIZE = 16 * GigaByte;

		private:
			RootAllocator(uint64 ReserveSize = ROOT_ALLOCATOR_DEFAULT_RESERVE_SIZE);

		public:
			INLINE static RootAllocator* Create(AllocatorBase* Allocator, uint64 ReserveSize)
			{
				if (__m_Instance == nullptr)
				{
					__m_Allocator = Allocator;
					if (__m_Allocator == nullptr)
						__m_Instance = new RootAllocator(ReserveSize);
					else
					{
						__m_Instance = ReinterpretCast(RootAllocator*, AllocateMemory(__m_Allocator, sizeof(RootAllocator)));
						new (__m_Instance) RootAllocator(ReserveSize);
					}
				}

				return __m_Instance;
			}

		private:
		};
	}
}

#endif