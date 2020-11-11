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
		class ProcedureAsLambda
		{
		public:
			ProcedureAsLambda(PlatformThread::Procedure& Procedure, void* Arguments) :
				m_Procedure(Procedure),
				m_Arguments(Arguments)
			{ }

			static uint32 Stub(void* Arguments)
			{
				ProcedureAsLambda* pThis = ReinterpretCast(ProcedureAsLambda*, Arguments);

				pThis->m_Procedure(pThis->m_Arguments);

				delete pThis;

				return 0;
			}

		private:
			PlatformThread::Procedure m_Procedure;
			void* m_Arguments;
		};

		PlatformThread::Handle PlatformThread::Create(Procedure Procedure, uint32 StackSize, void* Arguments, bool Suspended)
		{
			return _beginthreadex(0, StackSize, (_beginthreadex_proc_type)ProcedureAsLambda::Stub, new ProcedureAsLambda(Procedure, Arguments), (Suspended ? CREATE_SUSPENDED : 0), 0);
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

		void PlatformThread::Suspend(Handle Thread)
		{
			SuspendThread((HANDLE)Thread);
		}

		void PlatformThread::Resume(Handle Thread)
		{
			ResumeThread((HANDLE)Thread);
		}

		void PlatformThread::SetCoreAffinity(Handle Thread, uint8 CoreIndex)
		{
			SetThreadAffinityMask((HANDLE)Thread, 1 << CoreIndex);
		}

		PlatformThread::Handle PlatformThread::GetCurrentThread(void)
		{
			return (Handle)::GetCurrentThread();
		}

		uint32 PlatformThread::GetID(Handle Thread)
		{
			return (uint32)GetThreadId((HANDLE)Thread);
		}

		uint32 PlatformThread::GetID(void)
		{
			return (uint32)GetCurrentThreadId();
		}

		uint8 PlatformThread::GetHardwareConcurrency(void)
		{
			return std::thread::hardware_concurrency();
		}
	}
}
#endif