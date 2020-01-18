// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_H
#define MESH_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class Mesh
		{
		public:
			class SubMesh
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
				SubMesh(GPUBuffer BUffer, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout) :
					m_Buffer(BUffer),
					m_IndexCount(IndexCount),
					m_PolygonType(PolygonType),
					m_VertexLayout(VertexLayout)
				{
				}

				INLINE const GPUBuffer &GetBuffer(void) const
				{
					return m_Buffer;
				}

				INLINE uint16 GetVertexCount(void) const
				{
					return m_Buffer.GetCount();
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
				GPUBuffer m_Buffer;
				uint16 m_IndexCount;
				PolygonTypes m_PolygonType;
				VertexLayouts m_VertexLayout;
			};

		public:
			Mesh(SubMesh *SubMeshes, uint16 SubMeshCount) :
				m_SubMeshes(SubMeshes),
				m_SubMeshCount(SubMeshCount)
			{
			}

			INLINE SubMesh *GetSubMeshes(void) const
			{
				return m_SubMeshes;
			}

			INLINE uint16 GetSubMeshCount(void) const
			{
				return m_SubMeshCount;
			}

		private:
			SubMesh * m_SubMeshes;
			uint16 m_SubMeshCount;
		};

		typedef ResourceSystem::ResourceHandle<Mesh> MeshHandle;
	}
}

#endif