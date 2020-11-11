// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Fiber::Fiber(void) :
			m_Handle(0),
			m_ReturnHandle(0),
			m_ShouldExit(false)
		{
		}

		Fiber::~Fiber(void)
		{
			if (m_Handle == 0)
				return;

			Shutdown();
		}

		void Fiber::Initialize(PlatformFiber::Procedure Procedure, uint32 StackSize, void* Arguments)
		{
			Assert(m_Handle == 0, "Fiber already initialized");

			m_ShouldExit = false;
			m_ExitedPromiseBlock.Reset();

			m_Handle = PlatformFiber::Create([this, Procedure](void* Arguments)
				{
					Procedure(Arguments);
					Shutdown(true);
				}, StackSize, Arguments);

			m_ReturnHandle = 0;
		}

		Promise<void> Fiber::Shutdown(bool Force)
		{
			Assert(m_Handle != 0, "Fiber already shutted down");

			if (Force)
			{
				PlatformFiber::Delete(m_Handle);
				m_ExitedPromiseBlock.IncreaseDoneCount();

				m_Handle = 0;
				m_ReturnHandle = 0;
			}
			else
				m_ShouldExit = true;

			return &m_ExitedPromiseBlock;
		}

		void Fiber::Switch(void)
		{
			if (m_Handle == 0)
				return;

			m_ReturnHandle = 0;

			PlatformFiber::Switch(m_Handle);
		}

		void Fiber::SwitchTo(Fiber* Target)
		{
			if (Target->m_Handle == 0)
				return;

			Target->m_ReturnHandle = m_Handle;

			PlatformFiber::Switch(Target->m_Handle);
		}

		void Fiber::SwitchBack(void)
		{
			if (m_ReturnHandle == 0)
				return;

			PlatformFiber::Switch(m_ReturnHandle);
		}
	}
}