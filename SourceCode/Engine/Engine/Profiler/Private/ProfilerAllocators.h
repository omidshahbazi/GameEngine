// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILER_ALLOCATORS_H
#define PROFILER_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Profiler
	{
		namespace Private
		{
			class PROFILER_API ProfilerAllocators
			{
			public:
				CREATOR_DECLARATION(ProfilerAllocators);

			private:
				ProfilerAllocators(void);

			public:
				DEFINE_STATIC_ALLOCATOR_HELPERS(SampleDataAllocator)

			public:
				static DynamicSizeAllocator* SampleDataListAllocator;

			private:
				static FixedSizeAllocator* SampleDataAllocator;
			};
		}
	}
}

#endif