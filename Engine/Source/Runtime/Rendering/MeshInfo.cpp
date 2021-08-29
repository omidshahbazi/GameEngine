// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\MeshInfo.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		cstr SubMeshInfo::GetLayoutName(SubMesh::VertexLayouts Layout)
		{
			static bool initialized = false;
			static Map<SubMesh::VertexLayouts, cstr> registers;

			if (!initialized)
			{
				initialized = true;

				registers[SubMesh::VertexLayouts::Position] = "POSITION";
				registers[SubMesh::VertexLayouts::Normal] = "NORMAL";
				registers[SubMesh::VertexLayouts::TexCoord] = "TEXCOORD";
			}

			if (registers.Contains(Layout))
				return registers[Layout];

			return nullptr;
		}

		uint16 SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts Layout)
		{
			static bool initialized = false;
			static Map<SubMesh::VertexLayouts, uint16> registers;

			if (!initialized)
			{
				initialized = true;

				registers[SubMesh::VertexLayouts::Position] = 0;
				registers[SubMesh::VertexLayouts::Normal] = 1;
				registers[SubMesh::VertexLayouts::TexCoord] = 2;
			}

			if (registers.Contains(Layout))
				return registers[Layout];

			return -1;
		}

		Formats SubMeshInfo::GetLayoutFormat(SubMesh::VertexLayouts Layout)
		{
			switch (Layout)
			{
			case SubMesh::VertexLayouts::Position:
				return Formats::RGB32F;

			case SubMesh::VertexLayouts::Normal:
				return Formats::RGB32F;

			case SubMesh::VertexLayouts::TexCoord:
				return Formats::RG32F;
			}

			return Formats::R8;
		}

		uint16 SubMeshInfo::GetLayoutCount(void)
		{
			return 3;
		}
	}
}