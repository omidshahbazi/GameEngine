// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Thread::Thread(void) :
			m_Handle(0)
		{
		}

		Thread::~Thread(void)
		{
			if (m_Handle == 0)
				return;

			PlatformThread::End();

			m_Handle = 0;
		}

		void Thread::Initialize(PlatformThread::Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			m_Handle = PlatformThread::Begin(Procedure, StackSize, Arguments);
		}

		void Thread::Wait(void)
		{
			PlatformThread::Wait(m_Handle, PlatformThread::INFINITE_TIME);
		}

		void Thread::Join(void)
		{
			PlatformThread::Join(m_Handle);
		}

		void Thread::Sleep(uint64 Milliseconds)
		{
			PlatformThread::Sleep(Milliseconds);
		}

		void Thread::SetCoreAffinity(uint32 CoreIndex)
		{
			PlatformThread::SetCoreAffinity(m_Handle, CoreIndex);
		}
	}
}