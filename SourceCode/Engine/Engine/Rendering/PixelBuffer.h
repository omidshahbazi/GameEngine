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
			void Lock(void);
			void Unlock(void);

			void Move(uint32 Count);

			ColorUI8 GetPixel(void) const;

		private:
			uint8 m_ChannelSize;
			uint8 m_ChannelCount;
		};
	}
}

#endif