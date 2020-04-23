// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\SubMesh.h>
#include <Rendering\IDevice.h>
#include <Rendering\VertexBuffer.h>
#include <Rendering\IndexBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SubMesh::SubMesh(IDevice* Device, NativeType::Handle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout) :
			NativeType(Device, Handle),
			m_VertexCount(VertexCount),
			m_IndexCount(IndexCount),
			m_PolygonType(PolygonType),
			m_VertexLayout(VertexLayout),
			m_IndexBuffer(nullptr),
			m_VertexBuffer(nullptr)
		{
			GenerateBuffers();
		}

		void SubMesh::GenerateBuffers(void)
		{
			GPUBuffer::Handle vertexBufferHandle;
			GetDevice()->GetMeshVertexBuffer(GetHandle(), vertexBufferHandle);
			m_VertexBuffer = ReinterpretCast(VertexBuffer*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(VertexBuffer)));
			Construct_Macro(VertexBuffer, m_VertexBuffer, this, vertexBufferHandle);

			GPUBuffer::Handle elementBufferHandle;
			GetDevice()->GetMeshElementBuffer(GetHandle(), elementBufferHandle);
			m_IndexBuffer = ReinterpretCast(IndexBuffer*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(IndexBuffer)));
			Construct_Macro(IndexBuffer, m_IndexBuffer, this, elementBufferHandle);
		}

		uint32 SubMesh::GetVertexSize(void)
		{
			return sizeof(Vertex);
		}

		uint32 SubMesh::GetIndexSize(void)
		{
			return sizeof(uint32);
		}

		uint32 SubMesh::GetVertexBufferSize(uint32 Count)
		{
			return GetVertexSize() * Count;
		}

		uint32 SubMesh::GetIndexBufferSize(uint32 Count)
		{
			return GetIndexSize() * Count;
		}
	}
}