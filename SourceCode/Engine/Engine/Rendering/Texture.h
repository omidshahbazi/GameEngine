// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Rendering\RenderingCommon.h>
#include <Rendering\NativeType.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace MathContainers;

	namespace Rendering
	{
		using namespace Private;

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
			Texture(ThreadedDevice* Device, Handle Handle, Types Type, Formats Format, const Vector2I& Dimension);

		public:
			virtual ~Texture(void);

			virtual void SetName(const WString& Name) override;

		public:
			virtual bool SetVerticalWrapping(WrapModes Mode);
			virtual bool SetHorizontalWrapping(WrapModes Mode);
			virtual bool SetMinifyFilter(MinifyFilters Filter);
			virtual bool SetMagnifyFilter(MagnfyFilters Filter);
			virtual bool GenerateMipMaps(void);

			INLINE Types GetType(void) const
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
				return GetChannelSize(m_Format);
			}

			INLINE uint8 GetChannelCount(void) const
			{
				return GetChannelCount(m_Format);
			}

			INLINE uint8 GetPixelSize(void) const
			{
				return GetPixelSize(m_Format);
			}

			INLINE uint32 GetRowPitch(void) const
			{
				return GetRowPitch(m_Format, m_Dimension.X);
			}

			INLINE uint32 GetBufferSize(void) const
			{
				return GetBufferSize(m_Format, m_Dimension);
			}

		public:
			static uint8 GetChannelSize(Formats Format);
			static uint8 GetChannelCount(Formats Format);
			static uint8 GetPixelSize(Formats Format);
			static uint32 GetRowPitch(Formats Format, int32 Width);
			static uint32 GetBufferSize(Formats Format, const Vector2I& Dimension);

		private:
			Types m_Type;
			Formats m_Format;
			Vector2I m_Dimension;
			PixelBuffer* m_Buffer;
		};
	}
}

#endif