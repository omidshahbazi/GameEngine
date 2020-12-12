// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\SubMesh.h>
#include <Rendering\VertexBuffer.h>
#include <Rendering\IndexBuffer.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SubMesh::SubMesh(ThreadedDevice* Device, NativeType::Handle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout) :
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

		SubMesh::~SubMesh(void)
		{
			if (m_IndexBuffer != nullptr)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(m_IndexBuffer);

			if (m_VertexBuffer != nullptr)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(m_VertexBuffer);
		}

		void SubMesh::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::VertextArray, GetName().GetValue());
		}

		void SubMesh::GenerateBuffers(void)
		{
			uint32 bufferSize = GetVertexBufferSize();
			if (bufferSize != 0)
			{
				GPUBuffer::Handle bufferHandle;
				if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
					return;

				if (!GetDevice()->CopyFromVertexToBuffer(bufferHandle, GPUBuffer::Types::Array, GPUBuffer::Usages::StaticCopy, GetHandle(), bufferSize).Wait())
					return;

				m_VertexBuffer = RenderingAllocators::RenderingSystemAllocator_Allocate<VertexBuffer>();
				ConstructMacro(VertexBuffer, m_VertexBuffer, this, bufferHandle);
			}

			bufferSize = GetIndexBufferSize();
			if (bufferSize != 0)
			{
				GPUBuffer::Handle bufferHandle;
				if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
					return;

				if (!GetDevice()->CopyFromIndexoBuffer(bufferHandle, GPUBuffer::Types::ElementArray, GPUBuffer::Usages::StaticCopy, GetHandle(), bufferSize).Wait())
					return;

				m_IndexBuffer = RenderingAllocators::RenderingSystemAllocator_Allocate<IndexBuffer>();
				ConstructMacro(IndexBuffer, m_IndexBuffer, this, bufferHandle);
			}
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