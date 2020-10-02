// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

			Shutdown();
		}

		void Thread::Initialize(PlatformThread::Procedure Procedure, uint32 StackSize, void* Arguments, bool Suspended)
		{
			m_Handle = PlatformThread::Create(Procedure, StackSize, Arguments, Suspended);
		}

		void Thread::Shutdown(void)
		{
			PlatformThread::Close(m_Handle);

			m_Handle = 0;
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

		void Thread::Sleep(uint64 Milliseconds)
		{
			PlatformThread::Sleep(Milliseconds);
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