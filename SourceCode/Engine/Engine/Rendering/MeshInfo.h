// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_INFO_H
#define MESH_INFO_H

#include <Rendering\Vertex.h>
#include <Containers\Vector.h>
#include <Rendering\Mesh.h>

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
				Type(SubMesh::PolygonTypes::TriangleStrip),
				Layout(SubMesh::VertexLayouts::Position)
			{
			}

			SubMeshInfo(AllocatorBase* Allocator) :
				Type(SubMesh::PolygonTypes::TriangleStrip),
				Layout(SubMesh::VertexLayouts::Position),
				Vertices(Allocator, 0),
				Indices(Allocator, 0)
			{
			}

			SubMeshInfo(const SubMeshInfo& Other) :
				Type(Other.Type),
				Layout(Other.Layout),
				Vertices(Other.Vertices),
				Indices(Other.Indices)
			{
			}

			SubMeshInfo& operator =(const SubMeshInfo& Other)
			{
				Type = Other.Type;
				Layout = Other.Layout;
				Vertices = Other.Vertices;
				Indices = Other.Indices;

				return *this;
			}

			static uint16 GetLayoutIndex(SubMesh::VertexLayouts Element);

		public:
			SubMesh::PolygonTypes Type;
			SubMesh::VertexLayouts Layout;

			Vector<Vertex> Vertices;
			Vector<uint32> Indices;
		};

		struct MeshInfo
		{
		public:
			MeshInfo(void) :
				m_IsInHeap(false)
			{
			}

			MeshInfo(AllocatorBase* Allocator) :
				SubMeshes(Allocator, 0),
				m_IsInHeap(true)
			{
			}

			~MeshInfo(void)
			{
				if (m_IsInHeap)
				{
					for each (auto mesh in SubMeshes)
						DeallocateMemory(SubMeshes.GetAllocator(), mesh);
				}

				SubMeshes.Clear();
			}

		public:
			Vector<SubMeshInfo*> SubMeshes;

		private:
			bool m_IsInHeap;
		};
	}
}

#endif