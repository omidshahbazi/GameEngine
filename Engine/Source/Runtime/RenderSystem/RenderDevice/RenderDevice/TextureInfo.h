// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_INFO_H
#define TEXTURE_INFO_H

#include <RenderCommon\Enums.h>
#include <Common\PrimitiveTypes.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Common;
	using namespace MathContainers;
	using namespace RenderCommon;

	namespace RenderDevice
	{
		struct TextureInfo
		{
		public:
			TextureInfo(void) :
				Type(TextureTypes::TwoD),
				Data(nullptr),
				Format(Formats::RGBA8)
			{
			}

		public:
			TextureTypes Type;
			Vector2I Dimension;
			const byte* Data;
			Formats Format;
			Vector4I Borders;
		};
	}
}

#endif