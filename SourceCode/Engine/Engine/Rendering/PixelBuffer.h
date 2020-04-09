// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API PixelBuffer : public GPUBuffer
		{
			friend class Texture;

		protected:
			PixelBuffer(IDevice* Device, Handle Handle, uint32 Size, uint8 ChannelSize, uint8 ChannelCount);

		public:
			void Lock(IDevice::BufferAccess Access = IDevice::BufferAccess::ReadOnly);
			void Unlock(void);

			void Move(uint32 Count);

			ColorUI8& GetColorUI8Pixel(void);
			const ColorUI8& GetColorUI8Pixel(void) const;

			ColorUI16& GetColorUI16Pixel(void);
			const ColorUI16& GetColorUI16Pixel(void) const;

			ColorUI32& GetColorUI32Pixel(void);
			const ColorUI32& GetColorUI32Pixel(void) const;

		private:
			uint8 m_ChannelSize;
			uint8 m_ChannelCount;
		};
	}
}

#endif