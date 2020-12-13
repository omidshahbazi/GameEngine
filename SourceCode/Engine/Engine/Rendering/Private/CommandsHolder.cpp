// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\CommandsHolder.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\Commands\CommandBase.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			using namespace Commands;

			const uint32 CommandPerQueueCount = 1000000;

#define IMPLEMENT_CLEAR_COMMANDS(Type) \
			for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i) \
			{ \
				auto& commands = *(m_##Type##CommandQueues[i]); \
				for (auto command : commands) \
					DestructMacro(CommandBase, command); \
				commands.Clear(); \
				m_##Type##CommandAllocators[i]->Reset(); \
			}

			CommandsHolder::CommandsHolder(void) :
				m_ShouldRender(false),
				m_CommandAllocators1
			{
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator)
			},
				m_CommandAllocators2
			{
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator),
				FrameAllocator("Command Allocator", RenderingAllocators::RenderingSystemAllocator)
			},
				m_CommandQueues1
			{
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount)
			},
				m_CommandQueues2
			{
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount),
				CommandList(RenderingAllocators::ContainersAllocator, CommandPerQueueCount)
			},
				m_FrontCommandAllocators{ nullptr, nullptr, nullptr, nullptr, nullptr },
				m_BackCommandAllocators{ nullptr, nullptr, nullptr, nullptr, nullptr },
				m_FrontCommandQueues{ nullptr, nullptr, nullptr, nullptr, nullptr },
				m_BackCommandQueues{ nullptr, nullptr, nullptr, nullptr, nullptr }
			{
				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					m_FrontCommandAllocators[i] = &m_CommandAllocators1[i];
					m_BackCommandAllocators[i] = &m_CommandAllocators2[i];

					m_FrontCommandQueues[i] = &m_CommandQueues1[i];
					m_BackCommandQueues[i] = &m_CommandQueues2[i];
				}
			}

			CommandsHolder::~CommandsHolder(void)
			{
				IMPLEMENT_CLEAR_COMMANDS(Back);
				IMPLEMENT_CLEAR_COMMANDS(Front);
			}

			void CommandsHolder::Swap(void)
			{
				m_Lock.Lock();

				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					FrameAllocator* tempAllocator = nullptr;

					tempAllocator = m_FrontCommandAllocators[i];
					m_FrontCommandAllocators[i] = m_BackCommandAllocators[i];
					m_BackCommandAllocators[i] = tempAllocator;

					CommandList* tempList = nullptr;

					tempList = m_FrontCommandQueues[i];
					m_FrontCommandQueues[i] = m_BackCommandQueues[i];
					m_BackCommandQueues[i] = tempList;
				}

				m_Lock.Release();

				m_ShouldRender = true;

				//IMPLEMENT_CLEAR_COMMANDS(Front);
				//for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				//{
				//	auto& commands = *(m_FrontCommandQueues[i]);
				//	for each (auto command in commands)
				//		DestructMacro(CommandBase, command);
				//	commands.Clear();
				//	m_FrontCommandAllocators[i]->Reset();
				//}
				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					auto& commands = *(m_FrontCommandQueues[i]);
					//for each (auto command in commands)
					//	DestructMacro(CommandBase, command);
					commands.Clear();
					m_FrontCommandAllocators[i]->Reset();
				}
			}
		}
	}
}