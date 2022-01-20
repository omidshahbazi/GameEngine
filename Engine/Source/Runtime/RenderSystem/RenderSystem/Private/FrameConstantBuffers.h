// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FRAME_CONSTANT_BUFFERS_H
#define FRAME_CONSTANT_BUFFERS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace RenderDevice
	{
		class IDevice;
	}

	using namespace Common;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		namespace Private
		{
			class GPUConstantBuffer;
			class ThreadedDevice;

			class FrameConstantBuffers
			{
			public:
				FrameConstantBuffers(ThreadedDevice* Device);
				~FrameConstantBuffers(void);

				void Reset(void);

				GPUConstantBuffer* Get(uint16 Size);

				void UploadBuffersToGPU(IDevice* Device) const;

			private:
				ThreadedDevice* m_Device;
				GPUConstantBuffer* m_Buffers;
				uint16 m_InitializedCap;
				uint16 m_BufferIndex;
			};
		}
	}
}

#endif