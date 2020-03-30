// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Texture.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		Texture::Texture(IDevice *Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension) :
			NativeType(Device, Handle),
			m_Type(Type),
			m_Format(Format),
			m_Dimension(Dimension)
		{
		}

		bool Texture::SetVerticalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTextureVerticalWrapping(GetHandle(), m_Type, Mode);
		}

		bool Texture::SetHorizontalWrapping(WrapModes Mode)
		{
			return GetDevice()->SetTextureHorizontalWrapping(GetHandle(), m_Type, Mode);
		}

		bool Texture::SetMinifyFilter(MinifyFilters Filter)
		{
			return GetDevice()->SetTextureMinifyFilter(GetHandle(), m_Type, Filter);
		}

		bool Texture::SetMagnifyFilter(MagnfyFilters Filter)
		{
			return GetDevice()->SetTextureMagnifyFilter(GetHandle(), m_Type, Filter);
		}

		bool Texture::GenerateMipMaps(void)
		{
			return GetDevice()->GenerateTextureMipMap(GetHandle(), m_Type);
		}
	}
}