// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Thread::Thread(void) :
			m_Handle(0),
			m_ShouldExit(false)
		{
		}

		Thread::~Thread(void)
		{
			if (m_Handle == 0)
				return;

			Shutdown();
		}

		void Thread::Initialize(PlatformThread::Procedure Procedure, uint32 StackSize, void* Arguments, bool Suspended)
		{
			HardAssert(m_Handle == 0, "Thread already initialized");

			m_ShouldExit = false;
			m_ExitedPromiseBlock.Reset();

			m_Handle = PlatformThread::Create([this, Procedure](void* Arguments)
				{
					Procedure(Arguments);
					Shutdown(true);
				}, StackSize, Arguments, Suspended);
		}

		Promise<void> Thread::Shutdown(bool Force)
		{
			HardAssert(m_Handle != 0, "Thread already shutted down");

			if (Force)
			{
				PlatformThread::Close(m_Handle);
				m_ExitedPromiseBlock.IncreaseDoneCount();

				m_Handle = 0;
			}
			else
				m_ShouldExit = true;

			return &m_ExitedPromiseBlock;
		}

		uint32 Thread::GetID(void) const
		{
			return PlatformThread::GetID(m_Handle);
		}

		String Thread::GetName(void) const
		{
			char8 value[1024];
			PlatformThread::GetDescription(m_Handle, value);

			return value;
		}

		void Thread::SetName(const String& Value)
		{
			PlatformThread::SetDescription(m_Handle, Value.GetValue());
		}

		void Thread::Join(void)
		{
			PlatformThread::Join(m_Handle);
		}

		void Thread::Suspend(void)
		{
			PlatformThread::Suspend(m_Handle);
		}

		void Thread::Resume(void)
		{
			PlatformThread::Resume(m_Handle);
		}

		void Thread::SetCoreAffinity(uint32 CoreIndex)
		{
			PlatformThread::SetCoreAffinity(m_Handle, CoreIndex);
		}
	}
}