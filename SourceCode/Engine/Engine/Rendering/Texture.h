// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Rendering\NativeType.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace MathContainers;

	namespace Rendering
	{
		class PixelBuffer;

		class RENDERING_API Texture : public NativeType
		{
			friend class DeviceInterface;
			friend class PixelBuffer;

		public:
			enum class Types
			{
				TwoD
			};

			enum class Formats
			{
				R8 = 0,
				R16,
				R32,
				R16F,
				R32F,
				RG8,
				RG16,
				RG32,
				RG16F,
				RG32F,
				RGB8,
				RGB16,
				RGB32,
				RGB16F,
				RGB32F,
				RGBA8,
				RGBA16,
				RGBA32,
				RGBA16F,
				RGBA32F,
				Depth16,
				Depth24,
				Depth32,
				Depth32F,
				Stencil24F,
				Stencil32F
			};

			enum class WrapModes
			{
				Clamp = 0,
				Repeat,
				ClampToEdge,
				MirroredRepeat
			};

			enum class MinifyFilters
			{
				Nearest = 0,
				Linear,
				NearestMipMapNearest,
				LinearMipMapNearest,
				NearestMipMapLinear,
				LinearMipMapLinear
			};

			enum class MagnfyFilters
			{
				Nearest = 0,
				Linear
			};

		protected:
			Texture(IDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension);
			virtual ~Texture(void);

		public:
			virtual bool SetVerticalWrapping(WrapModes Mode);
			virtual bool SetHorizontalWrapping(WrapModes Mode);
			virtual bool SetMinifyFilter(MinifyFilters Filter);
			virtual bool SetMagnifyFilter(MagnfyFilters Filter);
			virtual bool GenerateMipMaps(void);

			Types GetType(void) const
			{
				return m_Type;
			}

			Formats GetFormat(void) const
			{
				return m_Format;
			}

			const Vector2I& GetDimension(void) const
			{
				return m_Dimension;
			}

			PixelBuffer* GetBuffer(void)
			{
				return m_Buffer;
			}

			const PixelBuffer* GetBuffer(void) const
			{
				return m_Buffer;
			}

		protected:
			virtual void GenerateBuffer(void);

			uint8 GetChannelSize(void) const
			{
				return GetChannelSize(m_Format);
			}

			uint8 GetChannelCount(void) const
			{
				return GetChannelCount(m_Format);
			}

			uint32 GetBufferSize(void) const
			{
				return GetBufferSize(m_Format, m_Dimension);
			}

		public:
			static uint8 GetChannelSize(Formats Format);
			static uint8 GetChannelCount(Formats Format);
			static uint32 GetBufferSize(Formats Format, const Vector2I& Dimension);

		private:
			Types m_Type;
			Formats m_Format;
			Vector2I m_Dimension;
			PixelBuffer* m_Buffer;
		};

		typedef ResourceSystem::ResourceHandle<Texture> TextureHandle;
	}
}

#endif