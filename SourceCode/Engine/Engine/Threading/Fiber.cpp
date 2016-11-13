// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Threading\Fiber.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Threading
	{
		Fiber::Fiber(PlatformFiber::Procedure Procedure, uint32 StackSize, void *Arguments) :
			m_Handle(0)
		{
			m_Handle = PlatformFiber::Create(Procedure, StackSize, Arguments);
		}

		Fiber::~Fiber(void)
		{
			PlatformFiber::Delete(m_Handle);
		}

		void Fiber::Switch(void)
		{
			PlatformFiber::Switch(m_Handle);
		}
	}
}