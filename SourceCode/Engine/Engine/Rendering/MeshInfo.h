// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_INFO_H
#define MESH_INFO_H

#include <Rendering\Vertex.h>

namespace Engine
{
	using namespace Common;

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
				Vertex(nullptr),
				VertexCount(0),
				Indices(nullptr),
				IndexCount(0)
			{
			}

			static uint16 GetLayoutIndex(VertexLayouts Element);

		public:
			VertexLayouts Layout;

			Vertex *Vertex;
			uint32 VertexCount;

			uint32 *Indices;
			uint32 IndexCount;
		};

		struct MeshInfo
		{
		public:
			MeshInfo(void) :
				SubMeshes(nullptr),
				SubMeshCount(0)
			{
			}

			SubMeshInfo *SubMeshes;
			uint32 SubMeshCount;
		};
	}
}

#endif