// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FIBER_H
#define FIBER_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Promise.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;

	namespace Threading
	{
		class THREADING_API Fiber
		{
		public:
			Fiber(void);
			~Fiber(void);

			void Initialize(PlatformFiber::Procedure Procedure, uint32 StackSize = 0, void* Arguments = nullptr);
			Promise<void> Shutdown(bool Force = false);

			void Switch(void);
			void SwitchTo(Fiber* Target);
			void SwitchBack(void);

			bool GetShouldExit(void)
			{
				return m_ShouldExit;
			}

			bool GetHasExited(void)
			{
				return m_ExitedPromiseBlock.GetIsDone();
			}

		private:
			PlatformFiber::Handle m_Handle;
			PlatformFiber::Handle m_ReturnHandle;
			AtomicBool m_ShouldExit;
			PromiseBlock<void> m_ExitedPromiseBlock;
		};
	}
}

#endif