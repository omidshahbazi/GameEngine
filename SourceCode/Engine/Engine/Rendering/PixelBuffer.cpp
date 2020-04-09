// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\PixelBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		PixelBuffer::PixelBuffer(IDevice *Device, Handle Handle, uint32 Size, uint8 ChannelSize, uint8 ChannelCount) :
			GPUBuffer(Device, Handle, Size, IDevice::BufferTypes::PixelUnpack),
			m_ChannelSize(ChannelSize),
			m_ChannelCount(ChannelCount)
		{
		}

		void PixelBuffer::Lock(IDevice::BufferAccess Access)
		{
			GPUBuffer::Lock(Access);
		}

		void PixelBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();
		}

		void PixelBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(m_ChannelSize * m_ChannelCount * Count);
		}

		ColorUI8& PixelBuffer::GetColorUI8Pixel(void)
		{
			return *ReinterpretCast(ColorUI8*, GetCurrentBuffer());
		}

		const ColorUI8& PixelBuffer::GetColorUI8Pixel(void) const
		{
			return *ReinterpretCast(ColorUI8*, GetCurrentBuffer());
		}

		ColorUI16& PixelBuffer::GetColorUI16Pixel(void)
		{
			return *ReinterpretCast(ColorUI16*, GetCurrentBuffer());
		}

		const ColorUI16& PixelBuffer::GetColorUI16Pixel(void) const
		{
			return *ReinterpretCast(ColorUI16*, GetCurrentBuffer());
		}

		ColorUI32& PixelBuffer::GetColorUI32Pixel(void)
		{
			return *ReinterpretCast(ColorUI32*, GetCurrentBuffer());
		}

		const ColorUI32& PixelBuffer::GetColorUI32Pixel(void) const
		{
			return *ReinterpretCast(ColorUI32*, GetCurrentBuffer());
		}
	}
}