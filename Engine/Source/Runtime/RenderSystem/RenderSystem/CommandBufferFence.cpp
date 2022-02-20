// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\CommandBufferFence.h>
#include <RenderDevice\IFence.h>

namespace Engine
{
	namespace RenderSystem
	{
		CommandBufferFence::CommandBufferFence(IFence* Fence) :
			m_Fence(Fence)
		{
		}

		bool CommandBufferFence::GetIsPassed(void)
		{
			return m_Fence->GetIsPassed();
		}

		bool CommandBufferFence::Wait(void)
		{
			return m_Fence->Wait();
		}
	}
}