// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\Multithreading.h>
#include <thread>
#include <Windows.h>
#include <process.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		Multithreading::Handle Multithreading::Begin(Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			return _beginthread((_beginthread_proc_type)Procedure, StackSize, Arguments);
		}

		void Multithreading::End(void)
		{
			_endthread();
		}

		void Multithreading::Wait(Handle Thread, uint64 Milliseconds)
		{
			WaitForSingleObject((HANDLE)Thread, Milliseconds);
		}

		void Multithreading::Join(void)
		{
			Join();
		}

		void Multithreading::Sleep(uint64 Milliseconds)
		{
			::Sleep(Milliseconds);
		}

		void Multithreading::SetCoreAffinity(Handle Thread, uint8 CoreIndex)
		{
			SetThreadAffinityMask((HANDLE)Thread, 1 << CoreIndex);
		}

		uint8 Multithreading::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}