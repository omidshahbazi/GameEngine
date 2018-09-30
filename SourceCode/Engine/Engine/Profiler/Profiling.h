// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILING_H
#define PROFILING_H

#include <Profiler\ProfileSample.h>
#include <Profiler\RealtimeProfiler.h>

namespace Engine
{
	namespace Profiler
	{
#if DEBUG_MODE

#define BeginProfilerFrame() RealtimeProfiler::GetInstance()->BeginFrame()
#define EndProfilerFrame() RealtimeProfiler::GetInstance()->EndFrame()

#define ProfileScope(SampleName) ProfileSample __ps(MODULE_NAME, SampleName)
#define ProfileFunction() ProfileSample __ps(MODULE_NAME, __FUNCSIG__)

#else

#define BeginProfiler()
#define EndProfiler()

#define ProfileScope(SmapleName)
#define ProfileFunction()

#endif
	}
}

#endif