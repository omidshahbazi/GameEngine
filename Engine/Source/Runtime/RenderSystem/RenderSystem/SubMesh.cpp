// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\SubMesh.h>
#include <RenderSystem\VertexBuffer.h>
#include <RenderSystem\IndexBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		using namespace Private;

		SubMesh::SubMesh(ThreadedDevice* Device, ResourceHandle Handle, uint16 VertexCount, uint16 IndexCount, PolygonTypes PolygonType, VertexLayouts VertexLayout) :
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
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_IndexBuffer);

			if (m_VertexBuffer != nullptr)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_VertexBuffer);
		}

		void SubMesh::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Mesh, GetName().GetValue());
		}

		void SubMesh::GenerateBuffers(void)
		{
			uint32 bufferSize = GetVertexBufferSize();
			if (bufferSize != 0)
			{
				ResourceHandle bufferHandle;
				if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
					return;

				if (!GetDevice()->CopyFromVertexToBuffer(bufferHandle, GetHandle(), bufferSize).Wait())
					return;

				m_VertexBuffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<VertexBuffer>();
				ConstructMacro(VertexBuffer, m_VertexBuffer, this, bufferHandle);
			}

			bufferSize = GetIndexBufferSize();
			if (bufferSize != 0)
			{
				ResourceHandle bufferHandle;
				if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
					return;

				if (!GetDevice()->CopyFromIndexToBuffer(bufferHandle, GetHandle(), bufferSize).Wait())
					return;

				m_IndexBuffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<IndexBuffer>();
				ConstructMacro(IndexBuffer, m_IndexBuffer, this, bufferHandle);
			}
		}
	}
}