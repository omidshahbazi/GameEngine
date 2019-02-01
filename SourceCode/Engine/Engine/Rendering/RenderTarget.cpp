// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderTarget.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		RenderTarget::RenderTarget(IDevice *Device, Handle Handle) :
			Texture(Device, Handle)
		{
		}
	}
}