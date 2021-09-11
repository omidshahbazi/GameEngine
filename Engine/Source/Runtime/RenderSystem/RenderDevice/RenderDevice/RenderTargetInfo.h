// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TARGET_INFO_H
#define RENDER_TARGET_INFO_H

#include <Containers\Vector.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderDevice
	{
		struct RenderTextureInfo
		{
		public:
			RenderTextureInfo(void) :
				Format(Formats::RG8),
				Point(AttachmentPoints::Color0)
			{
			}

		public:
			Vector2I Dimension;
			Formats Format;
			AttachmentPoints Point;
		};

		struct RenderTargetInfo
		{
			Vector<RenderTextureInfo> Textures;
		};
	}
}

#endif