// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMANDS_HOLDER_H
#define COMMANDS_HOLDER_H

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

			public:
				CommandsHolder(void);
				~CommandsHolder(void);

				void Swap(void);

				FrameAllocator** GetFrontAllocators(void)
				{
					return m_FrontCommandAllocators;
				}

				CommandList** GetFrontCommandQueue(void)
				{
					return m_FrontCommandQueues;
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

				CommandList** GetBackCommandQueue(void)
				{
					return m_BackCommandQueues;
				}

			private:
				SpinLock m_Lock;
				bool m_ShouldRender;

				FrameAllocator m_CommandAllocators1[(int8)RenderQueues::COUNT];
				FrameAllocator m_CommandAllocators2[(int8)RenderQueues::COUNT];

				CommandList m_CommandQueues1[(int8)RenderQueues::COUNT];
				CommandList m_CommandQueues2[(int8)RenderQueues::COUNT];

				FrameAllocator* m_FrontCommandAllocators[(int8)RenderQueues::COUNT];
				FrameAllocator* m_BackCommandAllocators[(int8)RenderQueues::COUNT];

				CommandList* m_FrontCommandQueues[(int8)RenderQueues::COUNT];
				CommandList* m_BackCommandQueues[(int8)RenderQueues::COUNT];
			};
		}
	}
}

#endif