// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class DefaultAllocator;
			class CustomAllocator;

			const uint32 MAX_ALLOCATORS_COUNT = 1024;

			class MEMORYMANAGEMENT_API Initializer
			{
				friend class DefaultAllocator;
				friend class CustomAllocator;

			public:
				struct AllocatorInfo
				{
				public:
					cstr Name;
					float32 ReserveSizeRate;
				};

				CREATOR_DECLARATION(Initializer);
				GET_INSTANCE_DECLARATION(Initializer);

			private:
				Initializer(void) :
					m_ResevedSize(0),
					m_AllocatorsInfo(),
					m_AllocatorInfoCount(0)
				{
				}

			public:
				void Initialize(uint32 ReserveSize, const AllocatorInfo* const AllocatorsInfo, uint32 AllocatorInfoCount);

			private:
				INLINE uint32 GetReservedSize(void) const
				{
					return m_ResevedSize;
				}

				float32 GetReserveSizeRate(cstr Name) const;

			private:
				uint32 m_ResevedSize;
				AllocatorInfo m_AllocatorsInfo[MAX_ALLOCATORS_COUNT];
				uint32 m_AllocatorInfoCount;
			};
		}
	}
}

#endif