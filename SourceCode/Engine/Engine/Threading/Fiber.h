// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FIBER_H
#define FIBER_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		class THREADING_API Fiber
		{
		public:
			Fiber(void);
			~Fiber(void);

			void Initialize(PlatformFiber::Procedure Procedure, uint32 StackSize, void *Arguments);

			void Switch(void);

			void *GetData(void);

			void ConvertThreadToFiber(void *Arguments);

		private:
			PlatformFiber::Handle m_Handle;
		};
	}
}

#endif