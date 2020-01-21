// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformTime
		{
		public:
			static uint64 QueryPerformanceCounter(void);

			static uint64 QueryPerformanceFrequency(void);
		};
	}
}

#endif