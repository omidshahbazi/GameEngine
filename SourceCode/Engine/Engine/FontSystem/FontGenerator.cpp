// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontGenerator.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <Rendering\MeshInfo.h>
#include <Utility\AssetParser\OBJParser.h>
#include <Utility\FileSystem.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
	using namespace Rendering;
	using namespace Utility;
	using namespace Utility::AssetParser;
	using namespace MemoryManagement::Allocator;

	namespace FontSystem
	{
		using namespace Private;

		float32 Area(const FT_Outline &Outline)
		{
			float32 A = 0.0F;

			for (int32 p = Outline.n_points - 1, q = 0; q < Outline.n_points; p = q++)
			{
				const FT_Vector &pval = Outline.points[p];
				const FT_Vector &qval = Outline.points[q];

				A += (pval.x * qval.y) - (qval.x * pval.y);
			}

			return (A * 0.5f);
		}

		bool InsideTriangle(const FT_Vector &A, const FT_Vector &B, const FT_Vector &C, const FT_Vector &P)
		{
			float32 ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
			float32 cCROSSap, bCROSScp, aCROSSbp;

			ax = C.x - B.x; ay = C.y - B.y;
			bx = A.x - C.x; by = A.y - C.y;
			cx = B.x - A.x; cy = B.y - A.y;
			apx = P.x - A.x; apy = P.y - A.y;
			bpx = P.x - B.x; bpy = P.y - B.y;
			cpx = P.x - C.x; cpy = P.y - C.y;

			aCROSSbp = ax * bpy - ay * bpx;
			cCROSSap = cx * apy - cy * apx;
			bCROSScp = bx * cpy - by * cpx;

			return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
		}

		bool Snip(const FT_Outline &Outline, int32 u, int32 v, int32 w, int32 n, const Vector<int32> &V)
		{
			int32 p;

			const FT_Vector &A = Outline.points[V[u]];
			const FT_Vector &B = Outline.points[V[v]];
			const FT_Vector &C = Outline.points[V[w]];

			if (0 > (((B.x - A.x) * (C.y - A.y)) - ((B.y - A.y) * (C.x - A.x))))
				return false;

			for (p = 0; p < n; p++)
			{
				if ((p == u) || (p == v) || (p == w))
					continue;
				const FT_Vector &P = Outline.points[V[p]];
				if (InsideTriangle(A, B, C, P))
					return false;
			}

			return true;
		}

		void MakeMeshFromOutline(const FT_Outline &Outline, MeshInfo &MeshInfo)
		{
			SubMeshInfo subMeshInfo;
			subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position;

			for (int32 i = 0; i < Outline.n_points; ++i)
			{
				const FT_Vector &point = Outline.points[i];

				subMeshInfo.Vertices.Add({ Vector3F(point.x, point.y, 0), Vector2F(0, 0) });
			}

			int32 n = subMeshInfo.Vertices.GetSize();
			if (n < 3)
				return;

			Vector<int32> V(n);
			if (Area(Outline) > 0)
			{
				for (int32 v = 0; v < n; v++)
					V.Add(v);
			}
			else
			{
				for (int32 v = 0; v < n; v++)
					V.Add((n - 1) - v);
			}

			int32 nv = n;
			int32 count = 2 * nv;
			for (int32 v = nv - 1; nv > 2; )
			{
				if ((count--) <= 0)
					break;

				int32 u = v;
				if (nv <= u)
					u = 0;
				v = u + 1;
				if (nv <= v)
					v = 0;
				int32 w = v + 1;
				if (nv <= w)
					w = 0;

				if (Snip(Outline, u, v, w, nv, V))
				{
					int32 a, b, c, s, t;
					a = V[u];
					b = V[v];
					c = V[w];
					subMeshInfo.Indices.Insert(0, a);
					subMeshInfo.Indices.Insert(0, b);
					subMeshInfo.Indices.Insert(0, c);
					for (s = v, t = v + 1; t < nv; s++, t++)
						V[s] = V[t];
					nv--;
					count = 2 * nv;
				}
			}

			MeshInfo.SubMeshes.Add(subMeshInfo);
		}

		FT_Library freeTypeLib;
		FT_Face face;

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&FontSystemAllocators::FontSystemAllocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&FontSystemAllocators::FontSystemAllocator, Ptr);
		}

		FontGenerator::FontGenerator(void)
		{
			FT_Init_FreeType(&freeTypeLib);
		}

		FontGenerator::~FontGenerator(void)
		{
			FT_Done_Face(face);
			FT_Done_FreeType(freeTypeLib);
		}

		void FontGenerator::LoadFont(const byte *Data, uint32 Size)
		{
			FT_New_Memory_Face(freeTypeLib, Data, Size, 0, &face);

			FT_Set_Pixel_Sizes(face, 0, 0);
		}

		void FontGenerator::Generate(void)
		{
			FrameAllocator fontAllocator("Font Generator Allocator", RootAllocator::GetInstance(), 200 * MegaByte);
			FrameAllocator glyphAllocator("Glyph Generator Allocator", RootAllocator::GetInstance(), 100 * MegaByte);

			uint32 glyphIndex;

			uint64 charCode = FT_Get_First_Char(face, &glyphIndex);

			ByteBuffer buffer(&fontAllocator, 200 * MegaByte);

			OBJParser objParser;

			while (glyphIndex != 0)
			{
				if (face->glyph->outline.n_points != 0)
				{
					ByteBuffer meshBuffer(&glyphAllocator, 100 * MegaByte);
					MeshInfo meshInfo;
					MakeMeshFromOutline(face->glyph->outline, meshInfo);

					objParser.Dump(meshBuffer, meshInfo);

					buffer.Append(charCode);
					buffer.Append(meshBuffer.GetSize());
					buffer.AppendBuffer(meshBuffer);
				}

				charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);

				FT_Load_Char(face, charCode, FT_LOAD_RENDER);

				glyphAllocator.Reset();
			}

			FileSystem::WriteAllBytes(L"D:/1.bin", buffer.GetBuffer(), buffer.GetSize());
		}
	}
}