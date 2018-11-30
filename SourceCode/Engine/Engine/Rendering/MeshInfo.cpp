// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\MeshInfo.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		uint16 SubMeshInfo::GetLayoutIndex(VertexLayouts Element)
		{
			static bool initialized = false;
			static Map<VertexLayouts, uint16> registers;

			if (!initialized)
			{
				initialized = true;

				registers[VertexLayouts::Position] = 0;
				registers[VertexLayouts::Normal] = 1;
				registers[VertexLayouts::UV] = 2;
			}

			if (registers.Contains(Element))
				return registers[Element];

			return -1;
		}
	}
}