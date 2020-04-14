// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <Rendering\GPUBuffer.h>
#include <Containers\Color.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class Texture;

		class RENDERING_API PixelBuffer : public GPUBuffer
		{
			friend class Texture;

		protected:
			PixelBuffer(Texture* Texture, Handle Handle);

		public:
			void Lock(Access Access = Access::ReadOnly);
			void Unlock(void);

			void Move(uint32 X, uint32 Y);
			void Move(uint32 Count);

			ColorUI8& GetColorUI8Pixel(void);
			const ColorUI8& GetColorUI8Pixel(void) const;

			ColorUI16& GetColorUI16Pixel(void);
			const ColorUI16& GetColorUI16Pixel(void) const;

			ColorUI32& GetColorUI32Pixel(void);
			const ColorUI32& GetColorUI32Pixel(void) const;

		private:
			Texture* m_Texture;
			Access m_LastLockAccess;
		};
	}
}

#endif