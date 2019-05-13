// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontGenerator.h>
#include <Rendering\MeshInfo.h>
#include <Utility\AssetParser\InternalModelParser.h>
#include <Utility\FileSystem.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <Platform\PlatformFile.h>
#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H


#include <FontSystem\Private\src\FTVectoriser.h>

namespace Engine
{
	using namespace Rendering;
	using namespace Utility;
	using namespace Utility::AssetParser;
	using namespace MemoryManagement::Allocator;
	using namespace Platform;

	namespace FontSystem
	{
		FT_Library freeTypeLib;
		FT_Face face;

		bool MakeMeshFromOutline(const FT_GlyphSlot &GlyphSlot, Vertex *WorkingVerticesBuffer, MeshInfo &MeshInfo, Vector2F &Size, Vector2F &Bearing, Vector2F &Advance)
		{
			FTVectoriser vectorizer(GlyphSlot);
			vectorizer.MakeMesh();
			auto mesh = vectorizer.GetMesh();

			for (int32 i = 0; i < mesh->TesselationCount(); ++i)
			{
				const FTTesselation* subMesh = mesh->Tesselation(i);

				if (subMesh->PointCount() == 0)
					continue;

				SubMeshInfo *subMeshInfoPtr = ReinterpretCast(SubMeshInfo*, AllocateMemory(MeshInfo.SubMeshes.GetAllocator(), sizeof(SubMeshInfo)));
				Construct(subMeshInfoPtr, MeshInfo.SubMeshes.GetAllocator());
				SubMeshInfo &subMeshInfo = *subMeshInfoPtr;

				subMeshInfo.Type = (Mesh::SubMesh::PolygonTypes)(subMesh->PolygonType() - 1);
				subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position;

				for (int32 j = 0; j < subMesh->PointCount(); ++j)
				{
					const FTPoint &point = subMesh->Point(j);

					WorkingVerticesBuffer[j] = { Vector3F(point.Xf(), point.Yf(), 0), Vector2F(0, 0) };
				}

				subMeshInfo.Vertices.AddRange(WorkingVerticesBuffer, subMesh->PointCount());

				MeshInfo.SubMeshes.Add(subMeshInfoPtr);
			}

			return true;
		}

		FontGenerator::FontGenerator(void)
		{
			FT_Init_FreeType(&freeTypeLib);
		}

		FontGenerator::~FontGenerator(void)
		{
			if (face != nullptr)
				FT_Done_Face(face);

			FT_Done_FreeType(freeTypeLib);
		}

		void FontGenerator::LoadFont(const byte *Data, uint32 Size)
		{
			if (face != nullptr)
				FT_Done_Face(face);

			FT_New_Memory_Face(freeTypeLib, Data, Size, 0, &face);

			FT_Set_Pixel_Sizes(face, 0, 0);
			//FT_Set_Char_Size(face, 0, 0, 0, 0);
		}

		void FontGenerator::Generate(const WString &Path)
		{
			const uint64 FONT_ALLOCATOR_SIZE = 500 * MegaByte;
			const uint64 GLYPH_ALLOCATOR_SIZE = 10 * MegaByte;
			const uint64 MESH_ALLOCATOR_SIZE = 10 * MegaByte;
			const uint16 VERTEX_COUNT = 2 * 1024;

			FrameAllocator fontAllocator("Font Generator Allocator", RootAllocator::GetInstance(), FONT_ALLOCATOR_SIZE);
			FrameAllocator glyphAllocator("Glyph Generator Allocator", RootAllocator::GetInstance(), GLYPH_ALLOCATOR_SIZE);
			FrameAllocator meshhAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), MESH_ALLOCATOR_SIZE);
			FixedSizeAllocator vertexAllocator("Vertex Generator Allocator", RootAllocator::GetInstance(), sizeof(Vertex), VERTEX_COUNT);

			Vertex *verticesBuffer = ReinterpretCast(Vertex*, AllocateMemory(&vertexAllocator, VERTEX_COUNT));

			uint32 glyphIndex;

			uint64 charCode = FT_Get_First_Char(face, &glyphIndex);

			ByteBuffer buffer(&fontAllocator, FONT_ALLOCATOR_SIZE);

			InternalModelParser modelParser;

			//PlatformFile::Handle charMapHandle = PlatformFile::Open(L"D:/1.csv", PlatformFile::OpenModes::Output, PlatformFile::Encodings::UTF8);

			Vector2F size;
			Vector2F bearing;
			Vector2F advance;

			while (glyphIndex != 0)
			{
				ByteBuffer meshBuffer(&glyphAllocator, GLYPH_ALLOCATOR_SIZE);
				MeshInfo meshInfo(&meshhAllocator);
				
				auto &glyph = face->glyph;

				if (MakeMeshFromOutline(glyph, verticesBuffer, meshInfo, size, bearing, advance))
				{
					if (meshInfo.SubMeshes.GetSize() != 0)
					{
						modelParser.Dump(meshBuffer, meshInfo);

						buffer.Append(charCode);

						buffer.Append(size.X);
						buffer.Append(size.Y);

						buffer.Append(bearing.X);
						buffer.Append(bearing.Y);

						buffer.Append(advance.X);
						buffer.Append(advance.Y);

						buffer.Append(meshBuffer.GetSize());
						buffer.AppendBuffer(meshBuffer);
					}
				}

				//PlatformFile::Write(charMapHandle, (char16)charCode);
				//PlatformFile::Write(charMapHandle, (char16)',');
				//PlatformFile::Write(charMapHandle, face->charmap->encoding);
				//PlatformFile::Write(charMapHandle, (char16)',');
				//PlatformFile::Write(charMapHandle, (char16)'\n');

				charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);

				FT_Load_Char(face, charCode, FT_LOAD_RENDER);

				glyphAllocator.Reset();
				meshhAllocator.Reset();
			}

			//PlatformFile::Close(charMapHandle);

			FileSystem::WriteAllBytes(Path.GetValue(), buffer.GetBuffer(), buffer.GetSize());
		}
	}
}