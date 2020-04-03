// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class PixelBuffer : public GPUBuffer
		{
			friend class Texture;

		protected:
			PixelBuffer(IDevice* Device, Handle Handle);
		};
	}
}

#endif