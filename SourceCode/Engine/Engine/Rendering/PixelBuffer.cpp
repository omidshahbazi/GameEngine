// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\PixelBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		PixelBuffer::PixelBuffer(IDevice *Device, Handle Handle) :
			GPUBuffer(Device, Handle, IDevice::BufferTypes::PixelPack, IDevice::BufferAccess::ReadOnly)
		{
		}
	}
}