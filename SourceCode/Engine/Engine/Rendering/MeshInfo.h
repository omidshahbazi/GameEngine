// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_INFO_H
#define MESH_INFO_H

#include <Rendering\Vertex.h>
#include <Containers\Vector.h>
#include <Mesh.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		struct SubMeshInfo
		{
		public:
			SubMeshInfo(void) :
				Layout(Mesh::SubMesh::VertexLayouts::Position)
			{
			}

			static uint16 GetLayoutIndex(Mesh::SubMesh::VertexLayouts Element);

		public:
			Mesh::SubMesh::VertexLayouts Layout;

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