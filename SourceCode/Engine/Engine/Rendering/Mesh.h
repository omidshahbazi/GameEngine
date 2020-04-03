// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_H
#define MESH_H

#include <Rendering\NativeType.h>

namespace Engine
{
	namespace Rendering
	{
		class Mesh
		{
		public:
			class SubMesh : public NativeType
			{
			public:
				enum class PolygonTypes
				{
					Lines = 0,
					LineLoop,
					LineStrip,
					Triangles,
					TriangleStrip,
					TriangleFan,
					Quads,
					QuadStrip,
					Polygon
				};

				enum class VertexLayouts
				{
					Position = 2,
					Normal = 4,
					UV = 8
				};

			public:
				SubMesh(IDevice* Device, NativeType::Handle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout) :
					NativeType(Device, Handle),
					m_VertexCount(VertexCount),
					m_IndexCount(IndexCount),
					m_PolygonType(PolygonType),
					m_VertexLayout(VertexLayout)
				{
				}

				INLINE uint16 GetVertexCount(void) const
				{
					return m_VertexCount;
				}

				INLINE uint16 GetIndexCount(void) const
				{
					return m_IndexCount;
				}

				INLINE PolygonTypes GetPolygonType(void) const
				{
					return m_PolygonType;
				}

				INLINE VertexLayouts GetVertexLayout(void) const
				{
					return m_VertexLayout;
				}

			private:
				uint16 m_VertexCount;
				uint16 m_IndexCount;
				PolygonTypes m_PolygonType;
				VertexLayouts m_VertexLayout;
			};

		public:
			Mesh(SubMesh* SubMeshes, uint16 SubMeshCount) :
				m_SubMeshes(SubMeshes),
				m_SubMeshCount(SubMeshCount)
			{
			}

			INLINE SubMesh* GetSubMeshes(void) const
			{
				return m_SubMeshes;
			}

			INLINE uint16 GetSubMeshCount(void) const
			{
				return m_SubMeshCount;
			}

		private:
			SubMesh* m_SubMeshes;
			uint16 m_SubMeshCount;
		};

		typedef ResourceSystem::ResourceHandle<Mesh> MeshHandle;
	}
}

#endif