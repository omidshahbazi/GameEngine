// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SUB_MESH_H
#define SUB_MESH_H

#include <Rendering\NativeType.h>
#include <Rendering\Vertex.h>

namespace Engine
{
	namespace Rendering
	{
		class VertexBuffer;
		class IndexBuffer;

		class SubMesh : public NativeType
		{
			friend class VertexBuffer;
			friend class IndexBuffer;

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
			SubMesh(IDevice* Device, NativeType::Handle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout);
			virtual ~SubMesh(void);

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

			INLINE VertexBuffer* GetVertexBuffer(void)
			{
				return m_VertexBuffer;
			}

			INLINE const VertexBuffer* GetVertexBuffer(void) const
			{
				return m_VertexBuffer;
			}

			INLINE IndexBuffer* GetIndexBuffer(void)
			{
				return m_IndexBuffer;
			}

			INLINE const IndexBuffer* GetIndexBuffer(void) const
			{
				return m_IndexBuffer;
			}

		protected:
			virtual void GenerateBuffers(void);

			uint8 GetVertexBufferSize(void) const
			{
				return GetVertexBufferSize(m_VertexCount);
			}

			uint8 GetIndexBufferSize(void) const
			{
				return GetIndexBufferSize(m_VertexCount);
			}

		public:
			static uint32 GetVertexSize(void);
			static uint32 GetIndexSize(void);
			static uint32 GetVertexBufferSize(uint32 Count);
			static uint32 GetIndexBufferSize(uint32 Count);

		private:
			uint16 m_VertexCount;
			uint16 m_IndexCount;
			PolygonTypes m_PolygonType;
			VertexLayouts m_VertexLayout;
			VertexBuffer* m_VertexBuffer;
			IndexBuffer* m_IndexBuffer;
		};
	}
}

#endif