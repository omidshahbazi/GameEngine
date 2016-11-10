// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\Thread.h>
#include <process.h>

namespace Engine
{
	using namespace Common;

	namespace Threading
	{
		Thread::Thread(uint32 StackSize) :
			m_Handle(0),
			m_IsIdle(true),
			m_Procedure(nullptr)
		{
			m_Handle = _beginthread(&Thread::Procedure, StackSize, this);
		}

		void Thread::Do(ThreadProcedure Procedure)
		{
			if (m_IsIdle)
				return;

			m_Procedure = Procedure;
		}

		void Thread::Procedure(void *Arguments)
		{
			Thread *thread = reinterpret_cast<Thread*>(Arguments);


		}
	}
}