// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\PlatformFile.h>
#include <thread>
#include <Windows.h>
#include <process.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformFile::Handle PlatformFile::Begin(Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			return _beginthread((_beginthread_proc_type)Procedure, StackSize, Arguments);
		}

		void PlatformFile::End(void)
		{
			_endthread();
		}

		void PlatformFile::Wait(Handle Thread, uint64 Milliseconds)
		{
			WaitForSingleObject((HANDLE)Thread, Milliseconds);
		}

		void PlatformFile::Join(void)
		{
			Join();
		}

		void PlatformFile::Sleep(uint64 Milliseconds)
		{
			::Sleep(Milliseconds);
		}

		void PlatformFile::SetCoreAffinity(Handle Thread, uint8 CoreIndex)
		{
			SetThreadAffinityMask((HANDLE)Thread, 1 << CoreIndex);
		}

		uint8 PlatformFile::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}