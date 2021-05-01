// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TARGET_INFO_H
#define RENDER_TARGET_INFO_H

#include <Containers\Vector.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\RenderTarget.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;

	namespace Rendering
	{
		struct RenderTextureInfo
		{
		public:
			RenderTextureInfo(void) :
				Format(Formats::RG8),
				Point(RenderTarget::AttachmentPoints::Color0)
			{
			}

		public:
			Vector2I Dimension;
			Formats Format;
			RenderTarget::AttachmentPoints Point;
		};

		struct RenderTargetInfo
		{
			Vector<RenderTextureInfo> Textures;
		};
	}
}

#endif