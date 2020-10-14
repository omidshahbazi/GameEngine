// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMANDS_HOLDER_H
#define COMMANDS_HOLDER_H

#include <Rendering\RenderingCommon.h>
#include <Common\SpinLock.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			class RENDERING_API CommandsHolder
			{
			public:
				CommandsHolder(void);
				~CommandsHolder(void);

				void Lock(void)
				{
					m_Lock.Lock();
				}

				bool TryLock(void)
				{
					return m_Lock.TryLock();
				}

				void Release(void)
				{
					m_Lock.Release();
				}

				void Swap(void);

				CommandList** GetFrontCommandQueue(void)
				{
					return m_FrontCommandQueuesPtr;
				}

				CommandList** GetBackCommandQueue(void)
				{
					return m_BackCommandQueuesPtr;
				}

			private:
				SpinLock m_Lock;

				CommandList m_CommandQueues1[(int8)RenderQueues::COUNT];
				CommandList m_CommandQueues2[(int8)RenderQueues::COUNT];

				CommandList* m_FrontCommandQueuesPtr[(int8)RenderQueues::COUNT];
				CommandList* m_BackCommandQueuesPtr[(int8)RenderQueues::COUNT];
			};
		}
	}
}

#endif