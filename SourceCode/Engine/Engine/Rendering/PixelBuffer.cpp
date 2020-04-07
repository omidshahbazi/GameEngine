// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\PixelBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		PixelBuffer::PixelBuffer(IDevice *Device, Handle Handle, uint32 Size, uint8 ChannelSize, uint8 ChannelCount) :
			GPUBuffer(Device, Handle, Size, IDevice::BufferTypes::PixelPack, IDevice::BufferAccess::ReadOnly),
			m_ChannelSize(ChannelSize),
			m_ChannelCount(ChannelCount)
		{
		}

		void PixelBuffer::Lock(void)
		{
			GPUBuffer::Lock();
		}

		void PixelBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();
		}

		void PixelBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(m_ChannelSize * m_ChannelCount * Count);
		}

		ColorUI8 PixelBuffer::GetPixel(void) const
		{
			ColorUI8* col = ReinterpretCast(ColorUI8*, GetCurrentBuffer());

			return *col;
		}
	}
}