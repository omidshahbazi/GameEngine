// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\HighResolutionTime.h>
#include <Platform\PlatformTime.h>

namespace Engine
{
	using namespace Platform;

	namespace Utility
	{
		static uint64 Frequency = PlatformTime::QueryPerformanceFrequency();
		static uint64 StartCounter = PlatformTime::QueryPerformanceCounter();

		HighResolutionTime::Timestamp HighResolutionTime::GetTime(void)
		{
			uint64 counter = PlatformTime::QueryPerformanceCounter();
			counter -= StartCounter;

			counter *= 1000000;
			counter /= Frequency;

			return { counter };
		}
	}
}