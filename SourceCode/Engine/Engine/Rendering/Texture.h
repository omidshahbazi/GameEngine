// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Rendering\NativeType.h>

namespace Engine
{
	namespace Rendering
	{
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
				R8F,
				R16F,
				R32F,
				RG8,
				RG16,
				RG32,
				RGB,
				RGB8,
				RGB16,
				RGB32,
				RGBA,
				RGBA8,
				RGBA16,
				RGBA32
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
			Texture(IDevice *Device, Handle Handle);

		public:
			virtual bool SetVerticalWrapping(WrapModes Mode);
			virtual bool SetHorizontalWrapping(WrapModes Mode);
			virtual bool SetMinifyFilter(MinifyFilters Filter);
			virtual bool SetMagnifyFilter(MagnfyFilters Filter);
			virtual bool GenerateMipMaps(void);
		};
	}
}

#endif