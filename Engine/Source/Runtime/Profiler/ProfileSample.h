// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILE_SAMPLE_H
#define PROFILE_SAMPLE_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\List.h>
#include <Profiler\Private\ProfilerAllocators.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Profiler
	{
		using namespace Private;

		struct PROFILER_API SampleData
		{
		public:
			SampleData(void) :
				CallCount(0),
				EndCount(0),
				StartTime(0),
				Parent(nullptr),
				Children(ProfilerAllocators::SampleDataListAllocator, 0)
			{
			}

			~SampleData(void)
			{
				for (auto sample : Children)
					ProfilerAllocators::SampleDataAllocator_Deallocate(sample);
			}

			String ModuleName;
			String SampleName;

			uint32 CallCount;
			uint32 EndCount;

			uint64 StartTime;
			uint64 EndTime;

			SampleData* Parent;
			List<SampleData*> Children;
		};

		class PROFILER_API ProfileSample
		{
		public:
			ProfileSample(const String& ModuleName, const String& SampleName);
			~ProfileSample(void);
		};
	}
}

#endif