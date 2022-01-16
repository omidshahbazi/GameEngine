// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\GPUConstantBuffer.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			GPUConstantBuffer::GPUConstantBuffer(ThreadedDevice* Device, uint32 Size, ResourceHandle Handle) :
				GPUBuffer(Device, Handle, Size),
				ConstantBuffer(Size)
			{
			}

			void GPUConstantBuffer::Copy(const ConstantBuffer* const Other)
			{
				PlatformMemory::Copy(Other->m_CachedData, m_CachedData, Math::Min<uint16>(GPUBuffer::GetSize(), Other->GetSize()));
			}

			void GPUConstantBuffer::UploadToGPU(void)
			{
				GPUBuffer::Lock(GPUBufferAccess::WriteOnly, true);

				PlatformMemory::Copy(m_CachedData, GetCurrentBuffer(), GPUBuffer::GetSize());

				GPUBuffer::Unlock(true);
			}
		}
	}
}