// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Rendering\NativeType.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture : public NativeType
		{
		public:
			Texture(IDevice *Device, Handle Handle) :
				NativeType(Device, Handle)
			{
			}
		};
	}
}

#endif