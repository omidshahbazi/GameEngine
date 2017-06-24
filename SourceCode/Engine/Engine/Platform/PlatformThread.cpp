// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformThread.h>
#include <thread>
#include <Windows.h>
#include <process.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformThread::Handle PlatformThread::Begin(Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			return _beginthread((_beginthread_proc_type)Procedure, StackSize, Arguments);
		}

		void PlatformThread::End(void)
		{
			_endthread();
		}

		void PlatformThread::Wait(Handle Thread, uint64 Milliseconds)
		{
			WaitForSingleObject((HANDLE)Thread, Milliseconds);
		}

		void PlatformThread::Join(void)
		{
			Join();
		}

		void PlatformThread::Sleep(uint64 Milliseconds)
		{
			::Sleep(Milliseconds);
		}

		void PlatformThread::SetCoreAffinity(Handle Thread, uint8 CoreIndex)
		{
			SetThreadAffinityMask((HANDLE)Thread, 1 << CoreIndex);
		}

		uint8 PlatformThread::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}
#endif