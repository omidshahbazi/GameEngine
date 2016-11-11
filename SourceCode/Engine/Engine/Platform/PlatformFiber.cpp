// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\PlatformFiber.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformFiber::Handle PlatformFiber::Create(Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			return (PlatformFiber::Handle)CreateFiber(StackSize, (LPFIBER_START_ROUTINE)Procedure, Arguments);
		}

		void PlatformFiber::Delete(Handle Fiber)
		{
			DeleteFiber(Fiber);
		}

		void PlatformFiber::Switch(Handle Fiber)
		{
			SwitchToFiber(Fiber);
		}

		void *PlatformFiber::GetData(void)
		{
			return GetFiberData();
		}

		PlatformFiber::Handle PlatformFiber::ConvertThreadToFiber(void *Arguments)
		{
			return (PlatformFiber::Handle)::ConvertThreadToFiber(Arguments);
		}

		bool PlatformFiber::ConvertFiberToThread(void *Arguments)
		{
			return (::ConvertFiberToThread() == TRUE);
		}
	}
}