// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMANDS_HOLDER_H
#define COMMANDS_HOLDER_H

#include <RenderSystem\Private\Commands\IntermediateConstantBuffers.h>
#include <Common\SpinLock.h>
#include <Containers\Vector.h>
#include <RenderCommon\Enums.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace RenderCommon;
	using namespace MemoryManagement::Allocator;

	namespace RenderSystem
	{
		namespace Private
		{
			class ThreadedDevice;

			namespace Commands
			{
				class CommandBase;

				class CommandsHolder
				{
					friend class ThreadedDevice;

					static const uint8 CONTEXT_COUNT = 2;

				public:
					typedef Vector<CommandBase*> CommandList;

					struct Context
					{
					public:
						Context(void);

						FrameAllocator CommandAllocators[(int8)RenderQueues::COUNT];
						CommandList CommandQueues[(int8)RenderQueues::COUNT];
						IntermediateConstantBuffers Buffers;
					};

				public:
					CommandsHolder(void);
					~CommandsHolder(void);

					void Swap(void);

					FrameAllocator* GetFrontAllocators(void)
					{
						return m_Context[m_FrontContextIndex].CommandAllocators;
					}

					CommandList* GetFrontCommandQueue(void)
					{
						return m_Context[m_FrontContextIndex].CommandQueues;
					}

					IntermediateConstantBuffers* GetFrontIntermediateConstantBuffers(void)
					{
						return &m_Context[m_FrontContextIndex].Buffers;
					}

				private:
					bool TryLock(void)
					{
						bool result = (m_ShouldRender && m_Lock.TryLock());

						if (result)
							m_ShouldRender = false;

						return result;
					}

					void Release(void)
					{
						m_Lock.Release();
					}

					CommandList* GetBackCommandQueue(void)
					{
						return m_Context[m_BackContextIndex].CommandQueues;
					}

				private:
					SpinLock m_Lock;
					bool m_ShouldRender;

					Context m_Context[CONTEXT_COUNT];

					uint8 m_BackContextIndex;
					uint8 m_FrontContextIndex;
				};
			}
		}
	}
}

#endif