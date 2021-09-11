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
			namespace Commands
			{
				class  GPUConstantBuffer : public GPUBuffer, private ConstantBuffer
				{
				public:
					GPUConstantBuffer(ThreadedDevice* Device, uint32 Size, ResourceHandle Handle);

					void Copy(const ConstantBuffer* const Other);

					void UploadToGPU(void);

					uint32 GetSize(void) const
					{
						return GPUBuffer::GetSize();
					}
				};
			}
		}
	}
}

#endif