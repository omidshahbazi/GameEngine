// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMAND_BUFFER_HELPER_H
#define COMMAND_BUFFER_HELPER_H

#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderDevice\ICommandBuffer.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace RenderSystem
	{
		namespace Private
		{
			class CommandBufferHelper
			{
			public:
				INLINE static bool SubmitAndDestroy(ThreadedDevice* Device, ICommandBuffer* CommandBuffer)
				{
					IFence* fence = nullptr;
					if (!Device->CreateFence(fence).Wait())
						return false;

					CommandBuffer->SignalFences(&fence, 1);

					if (!Device->SubmitCommandBuffers(&CommandBuffer, 1).Wait())
						return false;

					if (!fence->Wait())
						return false;

					if (!Device->DestroyFences(&fence, 1).Wait())
						return false;

					return Device->DestroyCommandBuffers(&CommandBuffer, 1).Wait();
				}
			};
		}
	}
}

#endif