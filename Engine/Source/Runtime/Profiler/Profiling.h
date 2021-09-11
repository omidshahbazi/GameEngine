// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILING_H
#define PROFILING_H

#include <Profiler\ProfileSample.h>
#include <Profiler\RealtimeProfiler.h>

namespace Engine
{
	namespace Profiler
	{
#ifdef DEBUG_MODE

#define BeginProfilerFrame() Profiler::RealtimeProfiler::GetInstance()->BeginFrame()
#define EndProfilerFrame() Profiler::RealtimeProfiler::GetInstance()->EndFrame()

#define ProfileScope(SampleName) Profiler::ProfileSample __ps(MODULE_NAME, SampleName)
#define ProfileFunction() Profiler::ProfileSample __ps(MODULE_NAME, __FUNCSIG__)

#else

#define BeginProfiler()
#define EndProfiler()

#define ProfileScope(SmapleName)
#define ProfileFunction()

#endif
	}
}

#endif