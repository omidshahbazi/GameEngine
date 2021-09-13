// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace MemoryManagement;

	namespace Allocators
	{
		class DefaultAllocator;
		class CustomAllocator;

		const uint32 MAX_ALLOCATOR_NAME_LENGTH = 64;
		const uint32 MAX_ALLOCATORS_COUNT = 1024;

		class ALLOCATORS_API Initializer
		{
			friend class DefaultAllocator;
			friend class CustomAllocator;

		public:
			struct AllocatorInfo
			{
			public:
				char8 Name[MAX_ALLOCATOR_NAME_LENGTH];
				float32 ReserveSizeRate;
			};

			CREATOR_DECLARATION(Initializer);
			GET_INSTANCE_DECLARATION(Initializer);

		private:
			Initializer(void) :
				m_Initialized(false),
				m_ResevedSize(0),
				m_AllocatorsInfo(),
				m_AllocatorInfoCount(0)
			{
			}

		public:
			void Initialize(uint64 ReserveSize, const AllocatorInfo* const AllocatorsInfo, uint32 AllocatorInfoCount);
			void Initialize(uint64 ReserveSize, cwstr FilePath);

			static uint32 ReadInfoFromFile(cwstr FilePath, AllocatorInfo* AllocatorsInfo, uint32 AllocatorInfoCount);

		private:
			INLINE uint64 GetReservedSize(void) const
			{
				return m_ResevedSize;
			}

			float32 GetReserveSizeRate(cstr Name) const;

		private:
			bool m_Initialized;

			uint64 m_ResevedSize;
			AllocatorInfo m_AllocatorsInfo[MAX_ALLOCATORS_COUNT];
			uint32 m_AllocatorInfoCount;
		};
	}
}

#endif