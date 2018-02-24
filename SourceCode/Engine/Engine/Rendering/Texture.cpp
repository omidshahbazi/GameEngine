// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Texture.h>

namespace Engine
{
	namespace Rendering
	{
		Texture::Texture(IDevice *Device, Handle Handle) :
			m_Device(Device),
			m_Handle(Handle)
		{
		}

		Texture::~Texture(void)
		{
		}
	}
}