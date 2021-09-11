// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformTime.h>
#include <Windows.h>

namespace Engine
{
	namespace Platform
	{
		uint64 PlatformTime::QueryPerformanceCounter(void)
		{
			LARGE_INTEGER value;
			::QueryPerformanceCounter(&value);
			return value.QuadPart;
		}

		uint64 PlatformTime::QueryPerformanceFrequency(void)
		{
			LARGE_INTEGER value;
			::QueryPerformanceFrequency(&value);
			return value.QuadPart;
		}
	}
}
#endif