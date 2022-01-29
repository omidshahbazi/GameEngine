// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <RenderSystem\Texture.h>

namespace Engine
{
	using namespace MathContainers;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API Sprite : public Texture
		{
			friend class DeviceInterface;

		private:
			Sprite(ThreadedDevice* Device, ResourceHandle Handle, Formats Format, const Vector2I& Dimension, const Vector4I& Borders);

		public:
			const Vector4I& GetBorders(void) const
			{
				return m_Borders;
			}

		private:
			Vector4I m_Borders;
		};
	}
}

#endif