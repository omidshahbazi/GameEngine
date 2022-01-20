// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_CONSTANT_BUFFER_H
#define GPU_CONSTANT_BUFFER_H

#include <RenderSystem\ConstantBuffer.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			class GPUConstantBuffer : public GPUBuffer, private ConstantBuffer
			{
				friend class FrameConstantBuffers;

			public:
				GPUConstantBuffer(ThreadedDevice* Device, uint32 Size, ResourceHandle Handle);

				void Copy(const ConstantBuffer* const Other);

				uint32 GetSize(void) const
				{
					return GPUBuffer::GetSize();
				}

				const byte* GetCachedData(void) const
				{
					return m_CachedData;
				}
			};
		}
	}
}

#endif