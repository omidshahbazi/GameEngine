// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\Fiber.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Fiber::Fiber(void) :
			m_Handle(0)
		{
		}

		Fiber::~Fiber(void)
		{
			if (m_Handle != 0)
				PlatformFiber::Delete(m_Handle);
		}


		void Fiber::Initialize(PlatformFiber::Procedure Procedure, uint32 StackSize, void *Arguments)
		{
			m_Handle = PlatformFiber::Create(Procedure, StackSize, Arguments);
		}

		void Fiber::Switch(void)
		{
			if (m_Handle != 0)
				PlatformFiber::Switch(m_Handle);
		}

		void *Fiber::GetData(void)
		{
			return PlatformFiber::GetData();
		}

		void Fiber::ConvertThreadToFiber(void *Arguments)
		{
			PlatformFiber::ConvertThreadToFiber(Arguments);
		}
	}
}