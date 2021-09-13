// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <TTFAssetParser\Private\MeshFontGenerator.h>
#include <TTFAssetParser\Private\TTFParserAllocators.h>
#include <Allocators\RootAllocator.h>
#include <Allocators\FixedSizeAllocator.h>
#include <TTFAssetParser\Private\src\FTVectoriser.h>
#include <TTFAssetParser\Private\src\FTGL\FTPoint.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace Allocators;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			const float32 GLYPH_PIXEL_HEIGHT = 72;
			const uint16 VERTEX_COUNT = 2 * 1024;

			MeshFontGenerator::MeshFontGenerator(const ByteBuffer& TTFBuffer) :
				FontGeneratorBase(TTFBuffer, GLYPH_PIXEL_HEIGHT)
			{
			}

			void MeshFontGenerator::Generate(FontInfo& FontInfo)
			{
				FontInfo.Size = GLYPH_PIXEL_HEIGHT;

				FixedSizeAllocator vertexAllocator("Vertex Generator Allocator", TTFParserAllocators::MeshGeneratorAllocator, sizeof(Vertex), VERTEX_COUNT);

				Vertex* verticesBuffer = ReinterpretCast(Vertex*, AllocateMemory(&vertexAllocator, VERTEX_COUNT));

				FontInfo::GlyphInfo glyphInfo;

				uint32 glyphIndex;
				GetFirstGlyph(glyphIndex, glyphInfo.CharCode);

				while (glyphIndex != 0)
				{
					LoadCharacterOutline(glyphInfo.CharCode);

					glyphInfo.MeshInfo = MeshInfo(TTFParserAllocators::MeshGeneratorAllocator);

					GetGlyphBoundingSize(glyphInfo.Size);
					GetGlyphBearing(glyphInfo.Bearing);
					GetGlyphAdvance(glyphInfo.Advance);

					GetGlyphMeshInfo(verticesBuffer, glyphInfo.MeshInfo);

					FontInfo.Glyphs.Add(glyphInfo);

					GetNextGlyph(glyphIndex, glyphInfo.CharCode);
				}
			}

			void MeshFontGenerator::GetGlyphMeshInfo(Vertex* WorkingVerticesBuffer, MeshInfo& MeshInfo)
			{
				FTVectoriser vectorizer(GetGlyph());
				vectorizer.MakeMesh();
				auto mesh = vectorizer.GetMesh();

				for (int32 i = 0; i < mesh->TesselationCount(); ++i)
				{
					const FTTesselation* subMesh = mesh->Tesselation(i);

					if (subMesh->PointCount() == 0)
						continue;

					SubMeshInfo* subMeshInfoPtr = ReinterpretCast(SubMeshInfo*, AllocateMemory(MeshInfo.SubMeshes.GetAllocator(), sizeof(SubMeshInfo)));
					Construct(subMeshInfoPtr, MeshInfo.SubMeshes.GetAllocator());
					SubMeshInfo& subMeshInfo = *subMeshInfoPtr;

					subMeshInfo.Type = (PolygonTypes)(subMesh->PolygonType() - 1);
					subMeshInfo.Layout = VertexLayouts::Position;

					for (int32 j = 0; j < subMesh->PointCount(); ++j)
					{
						const FTPoint& point = subMesh->Point(j);

						WorkingVerticesBuffer[j] = { Vector3F(point.Xf(), point.Yf(), 0), Vector2F(0, 0) };
					}

					subMeshInfo.Vertices.AddRange(WorkingVerticesBuffer, subMesh->PointCount());

					MeshInfo.SubMeshes.Add(subMeshInfoPtr);
				}
			}
		}
	}
}