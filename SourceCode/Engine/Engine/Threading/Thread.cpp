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

			Multithreading::End();

			m_Handle = 0;
		}

		void Thread::Initialize(Multithreading::Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			m_Handle = Multithreading::Begin(Procedure, StackSize, Arguments);
		}

		void Thread::Wait(void)
		{
			Multithreading::Wait(m_Handle, Multithreading::INFINITE_TIME);
		}

		void Thread::Join(void)
		{
			Multithreading::Join();
		}

		void Thread::Sleep(uint64 Milliseconds)
		{
			Multithreading::Sleep(Milliseconds);
		}

		void Thread::SetCoreAffinity(uint32 CoreIndex)
		{
			Multithreading::SetCoreAffinity(m_Handle, CoreIndex);
		}
	}
}