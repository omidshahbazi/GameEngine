// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Rendering\NativeType.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace Common;
	using namespace ResourceSystem;

	namespace Rendering
	{
		class Texture : public NativeType, public Resource
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