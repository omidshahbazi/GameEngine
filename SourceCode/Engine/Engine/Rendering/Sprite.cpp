// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Sprite.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		Sprite::Sprite(IDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension, const Vector4I& Borders) :
			Texture(Device, Handle, Type, Format, Dimension),
			m_Borders(Borders)
		{
		}
	}
}