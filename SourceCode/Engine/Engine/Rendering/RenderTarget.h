// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include <Rendering\Texture.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class SwitchRenderTargetCommand;
			}
		}

		class RenderTarget : public Texture
		{
			friend class SwitchRenderTargetCommand;

		public:
			enum class AttachmentPoints
			{
				Depth = 0,
				Stencil,
				Color0,
				Color1,
				Color2,
				Color3,
				Color4,
				Color5,
				Color6,
				Color7,
				Color8,
				Color9,
				Color10,
				Color11,
				Color12,
				Color13,
				Color14,
				Color16
			};

		public:
			RenderTarget(IDevice *Device, Handle Handle);
		};
	}
}

#endif