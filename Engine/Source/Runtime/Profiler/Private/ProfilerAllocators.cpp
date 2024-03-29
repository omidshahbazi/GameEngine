// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Profiler\Private\ProfilerAllocators.h>
#include <Allocators\RootAllocator.h>
#include <Profiler\RealtimeProfiler.h>

namespace Engine
{
	namespace Profiler
	{
		namespace Private
		{
			CREATOR_DEFINITION(ProfilerAllocators);

			DynamicSizeAllocator* ProfilerAllocators::SampleDataListAllocator = nullptr;
			FixedSizeAllocator* ProfilerAllocators::SampleDataAllocator = nullptr;

			ProfilerAllocators::ProfilerAllocators(void)
			{
				static DynamicSizeAllocator sampleDataListAllocator("SampleData List Allocator", RootAllocator::GetInstance(), 128 * MegaByte);
				SampleDataListAllocator = &sampleDataListAllocator;

				static FixedSizeAllocator sampleDataAllocator("SampleData Allocator", RootAllocator::GetInstance(), sizeof(SampleData), 100000);
				SampleDataAllocator = &sampleDataAllocator;
			}
		}
	}
}