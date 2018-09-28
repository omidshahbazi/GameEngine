// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROFILE_SAMPLE_H
#define PROFILE_SAMPLE_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Profiler
	{
		class PROFILER_API ProfileSample
		{
		public:
			ProfileSample(const String &ModuleName, const String &SampleName);
			~ProfileSample(void);
		};
	}
}

#endif