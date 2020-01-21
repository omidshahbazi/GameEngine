// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TARGET_INFO_H
#define RENDER_TARGET_INFO_H

#include <Rendering\RenderTarget.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		struct RenderTextureInfo
		{
		public:
			RenderTextureInfo(void) :
				Width(0),
				Height(0),
				Format(Texture::Formats::RG8),
				Point(RenderTarget::AttachmentPoints::Color0)
			{
			}

		public:
			uint32 Width;
			uint32 Height;
			Texture::Formats Format;
			RenderTarget::AttachmentPoints Point;
		};

		struct RenderTargetInfo
		{
			Vector<RenderTextureInfo> Textures;
		};
	}
}

#endif