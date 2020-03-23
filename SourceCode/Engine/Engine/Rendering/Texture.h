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
		//TODO: Add properties like dimension, info about 9slice here and in meta file, dimension added but the other should added, a bit problem exists in compiling again
		class Texture : public NativeType
		{
			friend class DeviceInterface;
			friend class Program;

		public:
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
			Texture(IDevice *Device, Handle Handle, Formats Format, const Vector2I& Dimension);

		public:
			virtual bool SetVerticalWrapping(WrapModes Mode);
			virtual bool SetHorizontalWrapping(WrapModes Mode);
			virtual bool SetMinifyFilter(MinifyFilters Filter);
			virtual bool SetMagnifyFilter(MagnfyFilters Filter);
			virtual bool GenerateMipMaps(void);

			Formats GetFormat(void) const
			{
				return m_Format;
			}

			const Vector2I& GetDimension(void) const
			{
				return m_Dimension;
			}

		private:
			Formats m_Format;
			Vector2I m_Dimension;
		};

		typedef ResourceSystem::ResourceHandle<Texture> TextureHandle;
	}
}

#endif