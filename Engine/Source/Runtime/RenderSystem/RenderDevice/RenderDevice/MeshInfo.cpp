// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderDevice\MeshInfo.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderDevice
	{
		cstr SubMeshInfo::GetLayoutName(VertexLayouts Layout)
		{
			static bool initialized = false;
			static Map<VertexLayouts, cstr> registers;

			if (!initialized)
			{
				initialized = true;

				registers[VertexLayouts::Position] = "POSITION";
				registers[VertexLayouts::Normal] = "NORMAL";
				registers[VertexLayouts::TexCoord] = "TEXCOORD";
			}

			if (registers.Contains(Layout))
				return registers[Layout];

			return nullptr;
		}

		uint16 SubMeshInfo::GetLayoutIndex(VertexLayouts Layout)
		{
			static bool initialized = false;
			static Map<VertexLayouts, uint16> registers;

			if (!initialized)
			{
				initialized = true;

				registers[VertexLayouts::Position] = 0;
				registers[VertexLayouts::Normal] = 1;
				registers[VertexLayouts::TexCoord] = 2;
			}

			if (registers.Contains(Layout))
				return registers[Layout];

			return -1;
		}

		Formats SubMeshInfo::GetLayoutFormat(VertexLayouts Layout)
		{
			switch (Layout)
			{
			case VertexLayouts::Position:
				return Formats::RGB32F;

			case VertexLayouts::Normal:
				return Formats::RGB32F;

			case VertexLayouts::TexCoord:
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