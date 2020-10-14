// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\CommandsHolder.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			const uint32 CommandPerQueueCount = 1000000;

			CommandsHolder::CommandsHolder(void)
			{
				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					m_CommandQueues1[i] = CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount);
					m_CommandQueues2[i] = CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount);

					m_FrontCommandQueuesPtr[i] = &m_CommandQueues1[i];
					m_BackCommandQueuesPtr[i] = &m_CommandQueues2[i];
				}
			}

			CommandsHolder::~CommandsHolder(void)
			{
			}

			void CommandsHolder::Swap(void)
			{
				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					CommandList* temp = nullptr;

					temp = m_FrontCommandQueuesPtr[i];
					m_FrontCommandQueuesPtr[i] = m_BackCommandQueuesPtr[i];
					m_BackCommandQueuesPtr[i] = temp;
				}
			}
		}
	}
}