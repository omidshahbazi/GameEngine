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

		using namespace Private;

		class RenderTarget : public NativeType
		{
			friend class ThreadedDevice;
			friend class SwitchRenderTargetCommand;

		public:
			typedef Vector<Texture*> TexturesList;

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
			RenderTarget(ThreadedDevice* Device, Handle Handle, const TexturesList& Textures);

			INLINE Texture* operator [] (uint8 Index)
			{
				return GetTexture(Index);
			}

			INLINE const Texture* operator [] (uint8 Index) const
			{
				return GetTexture(Index);
			}

			INLINE Texture* GetTexture(uint8 Index)
			{
				return m_Textures[Index];
			}

			INLINE const Texture* GetTexture(uint8 Index) const
			{
				return m_Textures[Index];
			}

			INLINE TexturesList GetTextures(void)
			{
				return m_Textures;
			}

			INLINE const TexturesList& GetTextures(void) const
			{
				return m_Textures;
			}

			INLINE static bool IsColorPoint(AttachmentPoints Point)
			{
				return (AttachmentPoints::Color0 <= Point && Point <= AttachmentPoints::Color15);
			}

			INLINE static bool IsDepthStencilPoint(AttachmentPoints Point)
			{
				return (Point == AttachmentPoints::Depth || Point == AttachmentPoints::Stencil);
			}

		private:
			TexturesList m_Textures;
		};
	}
}

#endif