// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FRAME_DATA_CHAIN_H
#define FRAME_DATA_CHAIN_H

#include <RenderSystem\Private\FrameConstantBuffers.h>
#include <Common\SpinLock.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		namespace Private
		{
			class ThreadedDevice;

			class FrameConstantBuffersPool
			{
				friend class ThreadedDevice;

			public:
				FrameConstantBuffersPool(ThreadedDevice* Device);
				~FrameConstantBuffersPool(void);

				FrameConstantBuffers* Get(void);
				void Back(FrameConstantBuffers* Buffers);

			private:
				ThreadedDevice* m_Device;
				SpinLock m_Lock;
				Stack<FrameConstantBuffers*> m_Buffers;
			};
		}
	}
}

#endif