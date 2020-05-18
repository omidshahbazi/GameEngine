// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_INFO_H
#define TEXTURE_INFO_H

#include <Common\PrimitiveTypes.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\Texture.h>

namespace Engine
{
	using namespace Common;
	using namespace MathContainers;

	namespace Rendering
	{
		struct TextureInfo
		{
		public:
			TextureInfo(void) :
				Type(Texture::Types::TwoD),
				Data(nullptr),
				Format(Texture::Formats::RGBA8)
			{
			}

		public:
			Texture::Types Type;
			Vector2I Dimension;
			const byte* Data;
			Texture::Formats Format;
			Vector4I Borders;
		};
	}
}

#endif