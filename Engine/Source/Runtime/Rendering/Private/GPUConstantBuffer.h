// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_CONSTANT_BUFFER_H
#define GPU_CONSTANT_BUFFER_H

#include <Rendering\ConstantBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			class RENDERING_API GPUConstantBuffer : public GPUBuffer, private ConstantBuffer
			{
			public:
				GPUConstantBuffer(ThreadedDevice* Device, uint32 Size, GPUBuffer::Handle Handle);

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

#endif