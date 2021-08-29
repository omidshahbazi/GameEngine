// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Profiler\ProfileSample.h>
#include <Profiler\RealtimeProfiler.h>

namespace Engine
{
	namespace Profiler
	{
		ProfileSample::ProfileSample(const String &ModuleName, const String &SampleName)
		{
			RealtimeProfiler::GetInstance()->BeginSmaple(ModuleName, SampleName);
		}

		ProfileSample::~ProfileSample(void)
		{
			RealtimeProfiler::GetInstance()->EndSample();
		}
	}
}