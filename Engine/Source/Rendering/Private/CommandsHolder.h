// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMANDS_HOLDER_H
#define COMMANDS_HOLDER_H

#include <Rendering\Private\IntermediateConstantBuffers.h>
#include <Rendering\RenderingCommon.h>
#include <Common\SpinLock.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			class ThreadedDevice;

			class RENDERING_API CommandsHolder
			{
				friend class ThreadedDevice;

				static const uint8 CONTEXT_COUNT = 2;

			public:
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

#endif