// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <Rendering\Texture.h>

namespace Engine
{
	using namespace MathContainers;

	namespace Rendering
	{
		class RENDERING_API Sprite : public Texture
		{
			friend class DeviceInterface;

		protected:
			Sprite(IDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension);
		};

		typedef ResourceSystem::ResourceHandle<Sprite> SpriteHandle;
	}
}

#endif