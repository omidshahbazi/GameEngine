// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\GPUConstantBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			GPUConstantBuffer::GPUConstantBuffer(ThreadedDevice* Device, uint32 Size, GPUBuffer::Handle Handle) :
				GPUBuffer(Device, Handle, Size, Types::Constant),
				ConstantBuffer(Size)
			{
			}

			void GPUConstantBuffer::Copy(const ConstantBuffer* const Other)
			{
				PlatformMemory::Copy(Other->m_CachedData, m_CachedData, Mathematics::Min<uint16>(GPUBuffer::GetSize(), Other->GetSize()));
			}

			void GPUConstantBuffer::UploadToGPU(void)
			{
				GPUBuffer::Lock(Access::WriteOnly, true);

				PlatformMemory::Copy(m_CachedData, GetCurrentBuffer(), GPUBuffer::GetSize());

				GPUBuffer::Unlock(true);
			}
		}
	}
}