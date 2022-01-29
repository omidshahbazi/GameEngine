// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <RenderCommon\RenderCommon.h>
#include <RenderSystem\NativeType.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\Helper.h>

namespace Engine
{
	using namespace MathContainers;
	using namespace RenderCommon;

	namespace RenderSystem
	{
		class PixelBuffer;

		class RENDERSYSTEM_API Texture : public NativeType
		{
			friend class DeviceInterface;
			friend class PixelBuffer;

		protected:
			Texture(ThreadedDevice* Device, ResourceHandle Handle, TextureTypes Type, Formats Format, const Vector2I& Dimension);

		public:
			virtual ~Texture(void);

			virtual void SetNameInternal(const WString& Name) override;

		public:
			virtual bool SetVerticalWrapping(TextureWrapModes Mode);
			virtual bool SetHorizontalWrapping(TextureWrapModes Mode);
			virtual bool SetMinifyFilter(TextureMinifyFilters Filter);
			virtual bool SetMagnifyFilter(TextureMagnfyFilters Filter);
			virtual bool GenerateMipMaps(void);

			INLINE TextureTypes GetType(void) const
			{
				return m_Type;
			}

			INLINE Formats GetFormat(void) const
			{
				return m_Format;
			}

			INLINE const Vector2I& GetDimension(void) const
			{
				return m_Dimension;
			}

			INLINE PixelBuffer* GetBuffer(void)
			{
				return m_Buffer;
			}

			INLINE const PixelBuffer* GetBuffer(void) const
			{
				return m_Buffer;
			}

		protected:
			virtual void GenerateBuffer(void);

			INLINE uint8 GetChannelSize(void) const
			{
				return Helper::GetTextureChannelSize(m_Format);
			}

			INLINE uint8 GetChannelCount(void) const
			{
				return Helper::GetTextureChannelCount(m_Format);
			}

			INLINE uint8 GetPixelSize(void) const
			{
				return Helper::GetTexturePixelSize(m_Format);
			}

			INLINE uint32 GetRowPitch(void) const
			{
				return Helper::GetTextureRowPitch(m_Format, m_Dimension.X);
			}

			INLINE uint32 GetBufferSize(void) const
			{
				return Helper::GetTextureBufferSize(m_Format, m_Dimension);
			}

		private:
			TextureTypes m_Type;
			Formats m_Format;
			Vector2I m_Dimension;
			PixelBuffer* m_Buffer;
		};
	}
}

#endif