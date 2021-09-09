// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Commands\CommandsHolder.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\Private\Commands\CommandBase.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				const uint32 CommandPerQueueCount = 1000000;

				CommandsHolder::Context::Context(void) :
					CommandAllocators
				{
					FrameAllocator("Command Allocator", RenderSystemAllocators::RenderSystemAllocator),
					FrameAllocator("Command Allocator", RenderSystemAllocators::RenderSystemAllocator),
					FrameAllocator("Command Allocator", RenderSystemAllocators::RenderSystemAllocator),
					FrameAllocator("Command Allocator", RenderSystemAllocators::RenderSystemAllocator),
					FrameAllocator("Command Allocator", RenderSystemAllocators::RenderSystemAllocator)
				},
					CommandQueues
				{
					CommandList(RenderSystemAllocators::ContainersAllocator, CommandPerQueueCount),
					CommandList(RenderSystemAllocators::ContainersAllocator, CommandPerQueueCount),
					CommandList(RenderSystemAllocators::ContainersAllocator, CommandPerQueueCount),
					CommandList(RenderSystemAllocators::ContainersAllocator, CommandPerQueueCount),
					CommandList(RenderSystemAllocators::ContainersAllocator, CommandPerQueueCount)
				}
				{
				}

				CommandsHolder::CommandsHolder(void) :
					m_ShouldRender(false),
					m_BackContextIndex(0),
					m_FrontContextIndex(0)
				{
					//for (int8 i = 0; i < CONTEXT_COUNT; ++i)
					//	m_Context[i].Buffers = IntermediateConstantBuffers()
				}

				CommandsHolder::~CommandsHolder(void)
				{
					for (int8 i = 0; i < CONTEXT_COUNT; ++i)
					{
						Context& context = m_Context[i];

						for (int8 j = 0; j < (int8)RenderQueues::COUNT; ++j)
						{
							CommandList& queue = context.CommandQueues[j];

							for (auto command : queue)
								DestructMacro(CommandBase, command);

							queue.Clear();

							context.CommandAllocators[j].Reset();
						}

						context.Buffers.Reset();
					}
				}

				void CommandsHolder::Swap(void)
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

						context.CommandQueues[i].Clear();
						context.CommandAllocators[i].Reset();
						context.Buffers.Reset();
					}
				}
			}
		}
	}
}