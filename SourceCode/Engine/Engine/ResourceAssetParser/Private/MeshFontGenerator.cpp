// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\MeshFontGenerator.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <ResourceAssetParser\Private\src\FTVectoriser.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace MemoryManagement::Allocator;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			void MeshFontGenerator::Generate(FontInfo& FontInfo)
			{
				const uint16 VERTEX_COUNT = 2 * 1024;

				FixedSizeAllocator vertexAllocator("Vertex Generator Allocator", RootAllocator::GetInstance(), sizeof(Vertex), VERTEX_COUNT);

				Vertex* verticesBuffer = ReinterpretCast(Vertex*, AllocateMemory(&vertexAllocator, VERTEX_COUNT));

				FontInfo::GlyphInfo glyphInfo;

				uint32 glyphIndex;
				GetFirstGlyph(glyphIndex, glyphInfo.CharCode);

				while (glyphIndex != 0)
				{
					glyphInfo.MeshInfo = MeshInfo(&ResourceAssetParserAllocators::MeshhAllocator);

					GetGlyphMeshInfo(verticesBuffer, glyphInfo.MeshInfo);

					GetGlyphSize(glyphInfo.Size);
					GetGlyphBearing(glyphInfo.Bearing);
					GetGlyphAdvance(glyphInfo.Advance);

					FontInfo.Glyphs.Add(glyphInfo);

					GetNextGlyph(glyphIndex, glyphInfo.CharCode);

					LoadCharacter(glyphInfo.CharCode);
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

					subMeshInfo.Type = (SubMesh::PolygonTypes)(subMesh->PolygonType() - 1);
					subMeshInfo.Layout = SubMesh::VertexLayouts::Position;

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