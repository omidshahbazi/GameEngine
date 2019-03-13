// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Profiler\Private\ProfilerAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Profiler\RealtimeProfiler.h>

namespace Engine
{
	namespace Profiler
	{
		namespace Private
		{
			FixedSizeAllocator ProfilerAllocators::SampleDataAllocator("SampleData Allocator", RootAllocator::GetInstance(), sizeof(SampleData), 10000);
		}
	}
}