// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Texture.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		Texture::Texture(IDevice *Device, Handle Handle) :
			NativeType(Device, Handle)
		{
		}

		bool Texture::SetVerticalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTexture2DVerticalWrapping(GetHandle(), Mode);
		}

		bool Texture::SetHorizontalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTexture2DHorizontalWrapping(GetHandle(), Mode);
		}

		bool Texture::SetMinifyFilter(MinifyFilters Filter)
		{
			return GetDevice()->SetTexture2DMinifyFilter(GetHandle(), Filter);
		}

		bool Texture::SetMagnifyFilter(MagnfyFilters Filter)
		{
			return GetDevice()->SetTexture2DMagnifyFilter(GetHandle(), Filter);
		}

		bool Texture::GenerateMipMaps(void)
		{
			return GetDevice()->GenerateMipMap(GetHandle());
		}
	}
}