// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformFiber.h>

#ifndef FIBER_H
#define FIBER_H

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		class Fiber
		{
		public:
			Fiber(PlatformFiber::Procedure Procedure, uint32 StackSize, void *Arguments);
			~Fiber(void);

			void Switch(void);

		private:
			PlatformFiber::Handle m_Handle;
		};
	}
}

#endif