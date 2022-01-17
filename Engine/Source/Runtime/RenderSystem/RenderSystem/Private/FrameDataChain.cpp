// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\FrameDataChain.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			FrameDataChain::FrameDataChain(ThreadedDevice* Device) :
				m_ShouldRender(false),
				m_Context{ {Device}, {Device} },
				m_BackContextIndex(0),
				m_FrontContextIndex(0)
			{
			}

			void FrameDataChain::Swap(void)
			{
				m_Lock.Lock();

				m_BackContextIndex = m_FrontContextIndex;
				if (++m_FrontContextIndex == CONTEXT_COUNT)
					m_FrontContextIndex = 0;

				m_Lock.Release();

				m_ShouldRender = true;

				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					Context& context = m_Context[m_FrontContextIndex];

					context.Buffers.Reset();
				}
			}
		}
	}
}