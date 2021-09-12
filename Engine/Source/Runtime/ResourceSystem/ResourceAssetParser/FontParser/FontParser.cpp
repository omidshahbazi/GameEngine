// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontParser\FontParser.h>
#include <MeshParser\MeshParser.h>
#include <TextureParser\TextureParser.h>
#include <FontParser\Private\FontParserAllocators.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		using namespace Private;

		void FontParser::Parse(const ByteBuffer& Buffer, FontInfo& FontInfo)
		{
			FontInfo.Size = Buffer.ReadValue<float32>();

			FontInfo.RenderType = (FontRenderTypes)Buffer.ReadValue<int32>();

			if (FontInfo.RenderType == FontRenderTypes::Texture)
			{
				uint64 textureDataSize = Buffer.ReadValue<uint64>();

				if (textureDataSize != 0)
				{
					const byte* meshData = Buffer.ReadValue(textureDataSize);

					TextureParser::Parse(ByteBuffer(ConstCast(byte*, meshData), textureDataSize), FontInfo.TextureInfo);
				}
			}

			uint32 glyphCount = Buffer.ReadValue<uint32>();

			FontInfo::GlyphInfo glyphInfo;
			for (uint32 i = 0; i < glyphCount; ++i)
			{
				glyphInfo.CharCode = Buffer.ReadValue<uint64>();

				glyphInfo.Size.X = Buffer.ReadValue<float32>();
				glyphInfo.Size.Y = Buffer.ReadValue<float32>();

				glyphInfo.Bearing.X = Buffer.ReadValue<float32>();
				glyphInfo.Bearing.Y = Buffer.ReadValue<float32>();

				glyphInfo.Advance.X = Buffer.ReadValue<float32>();
				glyphInfo.Advance.Y = Buffer.ReadValue<float32>();

				if (FontInfo.RenderType == FontRenderTypes::Mesh)
				{
					glyphInfo.MeshInfo = MeshInfo(FontParserAllocators::MeshGeneratorAllocator);

					uint64 meshDataSize = Buffer.ReadValue<uint64>();

					if (meshDataSize != 0)
					{
						const byte* meshData = Buffer.ReadValue(meshDataSize);

						MeshParser::Parse(ByteBuffer(ConstCast(byte*, meshData), meshDataSize), glyphInfo.MeshInfo);
					}
				}
				else if (FontInfo.RenderType == FontRenderTypes::Texture)
				{
					glyphInfo.Bounds.X = Buffer.ReadValue<float32>();
					glyphInfo.Bounds.Y = Buffer.ReadValue<float32>();
					glyphInfo.Bounds.Z = Buffer.ReadValue<float32>();
					glyphInfo.Bounds.W = Buffer.ReadValue<float32>();
				}

				FontInfo.Glyphs.Add(glyphInfo);
			}
		}

		uint64 FontParser::GetDumpSize(const FontInfo& FontInfo)
		{
			uint64 size = sizeof(float32) + sizeof(int32) + sizeof(uint32);

			if (FontInfo.RenderType == FontRenderTypes::Texture)
			{
				size += sizeof(uint64) + TextureParser::GetDumpSize(FontInfo.TextureInfo);
			}

			for (const auto& glyphInfo : FontInfo.Glyphs)
			{
				size += sizeof(uint64) + sizeof(Vector2F) + sizeof(Vector2F) + sizeof(Vector2F);

				if (FontInfo.RenderType == FontRenderTypes::Mesh)
				{
					size += sizeof(uint64) + MeshParser::GetDumpSize(glyphInfo.MeshInfo);
				}
				else if (FontInfo.RenderType == FontRenderTypes::Texture)
				{
					size += sizeof(Vector4F);
				}
			}

			return size;
		}

		void FontParser::Dump(ByteBuffer& Buffer, const FontInfo& FontInfo)
		{
			Buffer << FontInfo.Size;
			Buffer << (int32)FontInfo.RenderType;

			if (FontInfo.RenderType == FontRenderTypes::Texture)
			{
				Buffer << TextureParser::GetDumpSize(FontInfo.TextureInfo);

				TextureParser::Dump(Buffer, FontInfo.TextureInfo);
			}

			Buffer << FontInfo.Glyphs.GetSize();
			for (const auto& glyphInfo : FontInfo.Glyphs)
			{
				Buffer << glyphInfo.CharCode;

				Buffer << glyphInfo.Size.X;
				Buffer << glyphInfo.Size.Y;

				Buffer << glyphInfo.Bearing.X;
				Buffer << glyphInfo.Bearing.Y;

				Buffer << glyphInfo.Advance.X;
				Buffer << glyphInfo.Advance.Y;

				if (FontInfo.RenderType == FontRenderTypes::Mesh)
				{
					Buffer << MeshParser::GetDumpSize(glyphInfo.MeshInfo);

					MeshParser::Dump(Buffer, glyphInfo.MeshInfo);
				}
				else if (FontInfo.RenderType == FontRenderTypes::Texture)
				{
					Buffer << glyphInfo.Bounds.X;
					Buffer << glyphInfo.Bounds.Y;
					Buffer << glyphInfo.Bounds.Z;
					Buffer << glyphInfo.Bounds.W;
				}
			}
		}
	}
}