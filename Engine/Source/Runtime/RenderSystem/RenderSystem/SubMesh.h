// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SUB_MESH_H
#define SUB_MESH_H

#include <RenderSystem\NativeType.h>
#include <RenderCommon\Vertex.h>
#include <RenderCommon\Helper.h>

namespace Engine
{
	using namespace RenderCommon;

	namespace RenderSystem
	{
		class VertexBuffer;
		class IndexBuffer;

		using namespace Private;

		class RENDERSYSTEM_API SubMesh : public NativeType
		{
			friend class DeviceInterface;
			friend class VertexBuffer;
			friend class IndexBuffer;

		protected:
			SubMesh(ThreadedDevice* Device, ResourceHandle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout);

		public:
			virtual ~SubMesh(void);

			virtual void SetNameInternal(const WString& Name) override;

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
				return Helper::GetMeshVertexBufferSize(m_VertexCount);
			}

			uint8 GetIndexBufferSize(void) const
			{
				return Helper::GetMeshIndexBufferSize(m_VertexCount);
			}

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