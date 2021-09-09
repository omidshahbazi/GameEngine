// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_INFO_H
#define MESH_INFO_H

#include <RenderCommon\RenderCommon.h>
#include <RenderCommon\Enums.h>
#include <RenderCommon\Vertex.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;
	using namespace RenderCommon;
	using namespace Containers;

	namespace RenderDevice
	{
		struct RENDERDEVICE_API SubMeshInfo
		{
		public:
			typedef Vector<Vertex> VertexList;
			typedef Vector<uint32> IndexList;

			SubMeshInfo(void) :
				Type(PolygonTypes::TriangleStrip),
				Layout(VertexLayouts::Position)
			{
			}

			SubMeshInfo(AllocatorBase* Allocator) :
				Type(PolygonTypes::TriangleStrip),
				Layout(VertexLayouts::Position),
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

			static cstr GetLayoutName(VertexLayouts Layout);
			static uint16 GetLayoutIndex(VertexLayouts Layout);
			static Formats GetLayoutFormat(VertexLayouts Layout);
			static uint16 GetLayoutCount(void);

		public:
			PolygonTypes Type;
			VertexLayouts Layout;

			VertexList Vertices;
			IndexList Indices;
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

			MeshInfo(const MeshInfo& Other)
			{
				*this = Other;
			}

			~MeshInfo(void)
			{
				if (m_IsInHeap)
				{
					for (auto mesh : SubMeshes)
					{
						Destruct(mesh);
						DeallocateMemory(SubMeshes.GetAllocator(), mesh);
					}
				}

				SubMeshes.Clear();
			}

			MeshInfo& operator = (const MeshInfo& Other)
			{
				Construct(&SubMeshes, Other.SubMeshes.GetAllocator(), 0);
				SubMeshes = Other.SubMeshes;
				m_IsInHeap = Other.m_IsInHeap;

				return *this;
			}

		public:
			Vector<SubMeshInfo*> SubMeshes;

		private:
			bool m_IsInHeap;
		};
	}
}

#endif