// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_FENCE_H
#define COMMAND_BUFFER_FENCE_H

namespace Engine
{
	namespace RenderDevice
	{
		class IFence;
	}

	using namespace RenderDevice;

	namespace RenderSystem
	{
		class DeviceInterface;
		class CommandBuffer;

		class RENDERSYSTEM_API CommandBufferFence
		{
			friend class DeviceInterface;
			friend class CommandBuffer;

		private:
			CommandBufferFence(IFence* Fence);

		public:
			bool GetIsPassed(void);

			bool Wait(void);

		private:
			IFence* GetFence(void) const
			{
				return m_Fence;
			}

		private:
			IFence* m_Fence;
		};
	}
}

#endif