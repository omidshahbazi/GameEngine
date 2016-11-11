// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace Common;

	namespace Threading
	{
		Fiber::Fiber(PlatformFiber::Procedure Procedure, uint32 StackSize, void *Arguments)
		{
		}

		Fiber::~Fiber(void)
		{
		}
	}
}