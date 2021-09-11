// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Sprite.h>

namespace Engine
{
	namespace RenderSystem
	{
		Sprite::Sprite(ThreadedDevice* Device, ResourceHandle Handle, Formats Format, const Vector2I& Dimension, const Vector4I& Borders) :
			Texture(Device, Handle, TextureTypes::TwoD, Format, Dimension),
			m_Borders(Borders)
		{
			// Flip borders in Y axis
			int32 y = m_Borders.Y;
			m_Borders.Y = m_Borders.W;
			m_Borders.W = y;
		}
	}
}