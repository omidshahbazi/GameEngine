// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ENUMS_H
#define ENUMS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace RenderCommon
	{
		enum class DeviceTypes
		{
			OpenGL,
			DirectX12,
			Vulkan
		};

		const DeviceTypes DEVICE_TYPES[] = { DeviceTypes::OpenGL, DeviceTypes::DirectX12, DeviceTypes::Vulkan };
		const uint8 DEVICE_TYPE_COUNT = _countof(DEVICE_TYPES);

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
			DepthStencil24F,
			DepthStencil32F
		};

		enum class RenderQueues
		{
			Default = 0,
			Geometry,
			Lighting,
			Transparent,
			HUD,
			COUNT
		};

		enum class GPUBufferTypes
		{
			Constant = 0,
			Vertex,
			Index,
			Pixel
		};

		enum class GPUBufferAccess
		{
			ReadOnly = 0,
			WriteOnly,
			ReadAndWrite
		};

		enum class TextureTypes
		{
			TwoD
		};

		enum class TextureWrapModes
		{
			Clamp = 0,
			Repeat,
			ClampToEdge,
			MirroredRepeat
		};

		enum class TextureMinifyFilters
		{
			Nearest = 0,
			Linear,
			NearestMipMapNearest,
			LinearMipMapNearest,
			NearestMipMapLinear,
			LinearMipMapLinear
		};

		enum class TextureMagnfyFilters
		{
			Nearest = 0,
			Linear
		};

		enum class AttachmentPoints
		{
			Depth = 0,
			DepthStencil,
			Color0,
			Color1,
			Color2,
			Color3,
			Color4,
			Color5,
			Color6,
			Color7,
			Color8
		};

		enum class PolygonTypes
		{
			Lines = 0,
			LineLoop,
			LineStrip,
			Triangles,
			TriangleStrip,
			TriangleFan,
			Quads,
			QuadStrip,
			Polygon
		};

		enum class VertexLayouts
		{
			Position = 2,
			Normal = 4,
			TexCoord = 8
		};
	}
}

#endif