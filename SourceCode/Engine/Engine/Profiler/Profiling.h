// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILING_H
#define PROFILING_H

#include <Profiler\ProfileSample.h>

namespace Engine
{
	namespace Profiler
	{
#if DEBUG_MODE
#define Profile(SampleName) ProfileSample __ps(MODULE_NAME, SampleName)
#else
#define Profile(SmapleName)
#endif
	}
}

#endif