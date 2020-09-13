// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformThread.h>
#include <Common\CharacterUtility.h>
#include <thread>
#include <Windows.h>
#include <process.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformThread::Handle PlatformThread::Begin(Procedure Procedure, uint32 StackSize, void* Arguments)
		{
			return _beginthread((_beginthread_proc_type)Procedure, StackSize, Arguments);
		}

		void PlatformThread::End(void)
		{
			_endthread();
		}

		void PlatformThread::Close(Handle Thread)
		{
			CloseHandle((HANDLE)Thread);
		}

		void PlatformThread::GetDescription(Handle Thread, str Description)
		{
			wstr value = nullptr;

			GetThreadDescription((HANDLE)Thread, &value);

			CharacterUtility::ChangeType(value, Description);
		}

		void PlatformThread::SetDescription(Handle Thread, cstr Description)
		{
			char16 value[1024];
			CharacterUtility::ChangeType(Description, value);

			SetThreadDescription((HANDLE)Thread, value);
		}

		void PlatformThread::Wait(Handle Thread, uint64 Milliseconds)
		{
			WaitForSingleObject((HANDLE)Thread, Milliseconds);
		}

		void PlatformThread::Join(Handle Thread)
		{
			Wait(Thread, INFINITE);
		}

		void PlatformThread::Sleep(uint64 Milliseconds)
		{
			::Sleep(Milliseconds);
		}

		void PlatformThread::SetCoreAffinity(Handle Thread, uint8 CoreIndex)
		{
			SetThreadAffinityMask((HANDLE)Thread, 1 << CoreIndex);
		}

		uint32 PlatformThread::GetID(Handle Thread)
		{
			return (uint32)GetThreadId((HANDLE)Thread);
		}

		uint32 PlatformThread::GetID(void)
		{
			return (uint32)GetCurrentThread();
		}

		uint8 PlatformThread::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}
#endif