// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderCommon\Helper.h>
#include <RenderCommon\Vertex.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace DataUtility;

	namespace RenderCommon
	{
		uint32 Helper::GetRenderStateHash(const RenderState& State)
		{
			uint32 hash = 0;

			Hash::CRC32(&State, 1, hash);

			return hash;
		}

		void Helper::GetNormalizedColor(const ColorUI8& InColor, Vector4F& OutColor)
		{
			OutColor.X = InColor.R / 255.F;
			OutColor.Y = InColor.G / 255.F;
			OutColor.Z = InColor.B / 255.F;
			OutColor.W = InColor.A / 255.F;
		}

		void Helper::ReverseBuffer(const byte* Source, uint32 Length, uint32 RowPitch, byte* Destination)
		{
			const uint32 len = Length / RowPitch;
			const uint16 maxIndex = len - 1;
			for (uint32 y = 0; y < len; ++y)
				PlatformMemory::Copy(Source + (RowPitch * y), Destination + (RowPitch * (maxIndex - y)), RowPitch);
		}

		uint8 Helper::GetTextureChannelSize(Formats Format)
		{
			switch (Format)
			{
			case Formats::R8:
			case Formats::RG8:
			case Formats::RGB8:
			case Formats::RGBA8:
				return 1;

			case Formats::R16:
			case Formats::R16F:
			case Formats::Depth16:
			case Formats::RG16:
			case Formats::RG16F:
			case Formats::RGB16:
			case Formats::RGB16F:
			case Formats::RGBA16:
			case Formats::RGBA16F:
				return 2;

			case Formats::Depth24:
			case Formats::DepthStencil24F:
				return 3;

			case Formats::R32:
			case Formats::R32F:
			case Formats::Depth32:
			case Formats::Depth32F:
			case Formats::DepthStencil32F:
			case Formats::RG32:
			case Formats::RG32F:
			case Formats::RGB32:
			case Formats::RGB32F:
			case Formats::RGBA32:
			case Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint8 Helper::GetTextureChannelCount(Formats Format)
		{
			switch (Format)
			{
			case Formats::R8:
			case Formats::R16:
			case Formats::R16F:
			case Formats::Depth16:
			case Formats::Depth24:
			case Formats::DepthStencil24F:
			case Formats::R32:
			case Formats::R32F:
			case Formats::Depth32:
			case Formats::Depth32F:
			case Formats::DepthStencil32F:
				return 1;

			case Formats::RG8:
			case Formats::RG16:
			case Formats::RG16F:
			case Formats::RG32:
			case Formats::RG32F:
				return 2;

			case Formats::RGB8:
			case Formats::RGB16:
			case Formats::RGB16F:
			case Formats::RGB32:
			case Formats::RGB32F:
				return 3;

			case Formats::RGBA8:
			case Formats::RGBA16:
			case Formats::RGBA16F:
			case Formats::RGBA32:
			case Formats::RGBA32F:
				return 4;
			}

			return 0;
		}

		uint8 Helper::GetTexturePixelSize(Formats Format)
		{
			return GetTextureChannelSize(Format) * GetTextureChannelCount(Format);
		}

		uint32 Helper::GetTextureRowPitch(Formats Format, int32 Width)
		{
			return GetTexturePixelSize(Format) * Width;
		}

		uint32 Helper::GetTextureBufferSize(Formats Format, const Vector2I& Dimension)
		{
			return GetTextureRowPitch(Format, Dimension.X) * Dimension.Y;
		}

		uint32 Helper::GetMeshVertexSize(void)
		{
			return sizeof(Vertex);
		}

		uint32 Helper::GetMeshIndexSize(void)
		{
			return sizeof(uint32);
		}

		uint32 Helper::GetMeshVertexBufferSize(uint32 Count)
		{
			return GetMeshVertexSize() * Count;
		}

		uint32 Helper::GetMeshIndexBufferSize(uint32 Count)
		{
			return GetMeshIndexSize() * Count;
		}

		bool Helper::IsColorPoint(AttachmentPoints Point)
		{
			return (AttachmentPoints::Color0 <= Point && Point <= AttachmentPoints::Color8);
		}
	}
}