// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\MeshInfo.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		uint16 SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts Element)
		{
			static bool initialized = false;
			static Map<SubMesh::VertexLayouts, uint16> registers;

			if (!initialized)
			{
				initialized = true;

				registers[SubMesh::VertexLayouts::Position] = 0;
				registers[SubMesh::VertexLayouts::Normal] = 1;
				registers[SubMesh::VertexLayouts::UV] = 2;
			}

			if (registers.Contains(Element))
				return registers[Element];

			return -1;
		}
	}
}