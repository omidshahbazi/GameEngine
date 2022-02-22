// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <RenderSystem\Texture.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API RenderTarget : public NativeType
		{
			friend class DeviceInterface;

		public:
			typedef Vector<Texture*> TexturesList;

		private:
			RenderTarget(ThreadedDevice* Device, ResourceHandle Handle, const TexturesList& Textures);

			virtual void SetNameInternal(const WString& Name) override;

		public:
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

		private:
			TexturesList m_Textures;
		};
	}
}

#endif