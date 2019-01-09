// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_INFO_H
#define MESH_INFO_H

#include <Rendering\Vertex.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		struct SubMeshInfo
		{
		public:
			enum class VertexLayouts
			{
				Position = 2,
				Normal = 4,
				UV = 8
			};

		public:
			SubMeshInfo(void) :
				Layout(VertexLayouts::Position)
			{
			}

			static uint16 GetLayoutIndex(VertexLayouts Element);

		public:
			VertexLayouts Layout;

			Vector<Vertex> Vertices;
			Vector<uint32> Indices;
		};

		struct MeshInfo
		{
			Vector<SubMeshInfo> SubMeshes;
		};
	}
}

#endif