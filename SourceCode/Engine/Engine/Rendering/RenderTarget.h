// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include <Rendering\Texture.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class SwitchRenderTargetCommand;
			}
		}

		class RenderTarget : public NativeType
		{
			friend class SwitchRenderTargetCommand;

		public:
			typedef Vector<Texture> TexturesList;

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
				Color15
			};

		public:
			RenderTarget(IDevice *Device, Handle Handle, const TexturesList &Textures);

			INLINE Texture *operator [] (uint8 Index)
			{
				return GetTexture(Index);
			}

			INLINE const Texture *operator [] (uint8 Index) const
			{
				return GetTexture(Index);
			}

			INLINE Texture *GetTexture(uint8 Index)
			{
				return &m_Textures[Index];
			}

			INLINE const Texture *GetTexture(uint8 Index) const
			{
				return &m_Textures[Index];
			}

		private:
			TexturesList m_Textures;
		};
	}
}

#endif