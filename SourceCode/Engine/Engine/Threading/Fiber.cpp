// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Threading\Fiber.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Fiber::Fiber(void) :
			m_Handle(0),
			m_ReturnHandle(0)
		{
		}

		Fiber::~Fiber(void)
		{
			if (m_Handle != 0)
				PlatformFiber::Delete(m_Handle);
		}

		void Fiber::Initialize(PlatformFiber::Procedure Procedure, uint32 StackSize, void* Arguments)
		{
			m_Handle = PlatformFiber::Create(Procedure, StackSize, Arguments);
		}

		void Fiber::Run(void)
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

		void* Fiber::GetData(void)
		{
			return PlatformFiber::GetData();
		}

		void Fiber::ConvertThreadToFiber(void* Arguments)
		{
			PlatformFiber::ConvertThreadToFiber(Arguments);
		}
	}
}