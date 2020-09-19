// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformFiber.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformFiber::Handle PlatformFiber::Create(Procedure Procedure, uint32 StackSize, void* Arguments)
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

		bool PlatformFiber::IsRunningOnFiber(void)
		{
			return IsThreadAFiber();
		}

		PlatformFiber::Handle PlatformFiber::GetCurrentFiber(void)
		{
			return (PlatformFiber::Handle)::GetCurrentFiber();
		}

		void* PlatformFiber::GetData(void)
		{
			return GetFiberData();
		}

		PlatformFiber::Handle PlatformFiber::ConvertThreadToFiber(void* Arguments)
		{
			return (PlatformFiber::Handle)::ConvertThreadToFiber(Arguments);
		}
	}
}
#endif