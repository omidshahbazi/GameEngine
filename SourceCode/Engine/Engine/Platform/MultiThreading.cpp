// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\MultiThreading.h>
#include <thread>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		uint32 MultiThreading::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}