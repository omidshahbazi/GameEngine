// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\Private\TextureFontLoader.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <ResourceAssetParser\InternalModelParser.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace MemoryManagement::Allocator;

	namespace FontSystem
	{
		namespace Private
		{
			void TextureFontLoader::Load(const ByteBuffer& InBuffer, ByteBuffer& OutBuffer)
			{
				LoadFace(InBuffer);

				const uint64 GLYPH_ALLOCATOR_SIZE = 10 * MegaByte;
				const uint64 MESH_ALLOCATOR_SIZE = 10 * MegaByte;
				const uint16 VERTEX_COUNT = 2 * 1024;

				FrameAllocator glyphAllocator("Glyph Generator Allocator", RootAllocator::GetInstance(), GLYPH_ALLOCATOR_SIZE);
				FrameAllocator meshhAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), MESH_ALLOCATOR_SIZE);
				FixedSizeAllocator vertexAllocator("Vertex Generator Allocator", RootAllocator::GetInstance(), sizeof(Vertex), VERTEX_COUNT);

				Vertex* verticesBuffer = ReinterpretCast(Vertex*, AllocateMemory(&vertexAllocator, VERTEX_COUNT));

				uint32 glyphIndex;
				uint64 charCode;
				GetFirstGlyph(glyphIndex, charCode);

				InternalModelParser modelParser;

				Vector2F size;
				Vector2F bearing;
				Vector2F advance;

				while (glyphIndex != 0)
				{
					MeshInfo meshInfo(&meshhAllocator);

					GetGlyphMeshInfo(verticesBuffer, meshInfo);

					GetGlyphSize(size);
					GetGlyphBearing(bearing);
					GetGlyphAdvance(advance);

					OutBuffer.Append(charCode);

					OutBuffer.Append(size.X);
					OutBuffer.Append(size.Y);

					OutBuffer.Append(bearing.X);
					OutBuffer.Append(bearing.Y);

					OutBuffer.Append(advance.X);
					OutBuffer.Append(advance.Y);

					if (meshInfo.SubMeshes.GetSize() == 0)
						OutBuffer.Append((uint64)0);
					else
					{
						ByteBuffer meshBuffer(&glyphAllocator, GLYPH_ALLOCATOR_SIZE);
						modelParser.Dump(meshBuffer, meshInfo);
						OutBuffer.Append(meshBuffer.GetSize());
						OutBuffer.AppendBuffer(meshBuffer);
					}

					GetNextGlyph(glyphIndex, charCode);

					LoadCharacter(charCode);

					glyphAllocator.Reset();
					meshhAllocator.Reset();
				}
			}

			void TextureFontLoader::Create(const ByteBuffer& Buffer, Font* Font)
			{
				Font->m_RenderType = Font::RenderTypes::Mesh;

				auto& initialChars = Font->m_InitialCharacters;
				uint64 index = 0;

#define READ_VALUE(Type) \
				Buffer.ReadValue<Type>(index);\
				index += sizeof(Type);

				while (index < Buffer.GetSize())
				{
					uint64 charCode = READ_VALUE(uint64);

					Vector2F size;
					size.X = READ_VALUE(float32);
					size.Y = READ_VALUE(float32);

					Vector2F bearing;
					bearing.X = READ_VALUE(float32);
					bearing.Y = READ_VALUE(float32);

					Vector2F advance;
					advance.X = READ_VALUE(float32);
					advance.Y = READ_VALUE(float32);

					uint64 meshDataSize = READ_VALUE(uint64);

					MeshInfo* meshInfo = nullptr;
					if (meshDataSize != 0)
					{
						const byte* meshData = Buffer.ReadValue(index, meshDataSize);
						index += meshDataSize;

						meshInfo = FontSystemAllocators::AllocatorReference_Allocate<MeshInfo>();
						Construct(meshInfo, &FontSystemAllocators::FontSystemAllocator);

						InternalModelParser parser;
						parser.Parse(ByteBuffer(ConstCast(byte*, meshData), meshDataSize), *meshInfo);
					}

					initialChars.Add(charCode, { meshInfo, size, bearing, advance });
				}
#undef READ_VALUE
			}

			void TextureFontLoader::GetGlyphMeshInfo(Vertex* WorkingVerticesBuffer, MeshInfo& MeshInfo)
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