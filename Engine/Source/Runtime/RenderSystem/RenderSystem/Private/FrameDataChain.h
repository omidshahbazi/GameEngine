// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FRAME_DATA_CHAIN_H
#define FRAME_DATA_CHAIN_H

#include <RenderSystem\Private\FrameConstantBuffers.h>
#include <Common\SpinLock.h>
#include <Containers\Vector.h>
#include <RenderCommon\Enums.h>
#include <Allocators\FrameAllocator.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace RenderCommon;
	using namespace Allocators;

	namespace RenderSystem
	{
		namespace Private
		{
			class ThreadedDevice;

			class FrameDataChain
			{
				friend class ThreadedDevice;

				static const uint8 CONTEXT_COUNT = 2;

			public:
				struct Context
				{
				public:
					FrameConstantBuffers Buffers;
				};

			public:
				FrameDataChain(ThreadedDevice* Device);
				~FrameDataChain(void);

				void Swap(void);

				FrameConstantBuffers* GetFrontConstantBuffers(void)
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