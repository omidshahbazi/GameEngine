// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INSTANTIATION_H
#define INSTANTIATION_H

#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			CREATOR_DEFINITION(Initializer);

			//TODO: find a way to set this value before start for editor, launcher and tools
			void Initializer::Initialize(uint32 ReserveSize, const AllocatorInfo* const AllocatorsInfo, uint32 AllocatorInfoCount)
			{
				Assert(ReserveSize > 0, "ReserveSize cannot be zero");
				Assert(AllocatorsInfo != nullptr, "AllocatorsInfo cannot be null");
				Assert(AllocatorInfoCount > 0, "AllocatorInfoCount cannot be zero");
				Assert(AllocatorInfoCount <= MAX_ALLOCATORS_COUNT, "AllocatorInfoCount exceeds MAX_ALLOCATORS_COUNT");

				m_ResevedSize = ReserveSize;
				m_AllocatorInfoCount = AllocatorInfoCount;
				for (uint32 i = 0; i < m_AllocatorInfoCount; ++i)
					m_AllocatorsInfo[i] = AllocatorsInfo[i];

				DefaultAllocator::Create();
				RootAllocator::Create(DefaultAllocator::GetInstance());
			}

			float32 Initializer::GetReserveSizeRate(cstr Name) const
			{
				for (uint32 i = 0; i < m_AllocatorInfoCount; ++i)
				{
					auto info = m_AllocatorsInfo[i];

					if (strcmp(info.Name, Name) == 0)
						return info.ReserveSizeRate;
				}

				return 0;
			}
		}
	}
}

#endif