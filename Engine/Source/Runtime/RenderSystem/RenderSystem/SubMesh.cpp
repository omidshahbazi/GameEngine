// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\SubMesh.h>
#include <RenderSystem\VertexBuffer.h>
#include <RenderSystem\IndexBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderDevice\ICommandBuffer.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace RenderDevice;

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

			SetName("Mesh");
		}

		SubMesh::~SubMesh(void)
		{
			if (m_IndexBuffer != nullptr)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_IndexBuffer);

			if (m_VertexBuffer != nullptr)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_VertexBuffer);
		}

		void SubMesh::SetNameInternal(const WString& Name)
		{
			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Mesh, GetName().GetValue());

			if (m_VertexBuffer != nullptr)
				m_VertexBuffer->SetName(Name + L"_VertexBuffer");

			if (m_IndexBuffer != nullptr)
				m_IndexBuffer->SetName(Name + L"_IndexBuffer");
		}

		void SubMesh::GenerateBuffers(void)
		{
			ICommandBuffer* cb = nullptr;
			if (!GetDevice()->CreateCommandBuffer(cb).Wait())
				return;

			uint32 bufferSize = GetVertexBufferSize();
			if (bufferSize != 0)
			{
				ResourceHandle bufferHandle;
				if (!GetDevice()->CreateBuffer(GPUBufferTypes::Vertex, bufferSize, bufferHandle).Wait())
					return;

				cb->CopyBuffer(GetHandle(), bufferHandle);

				m_VertexBuffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<VertexBuffer>();
				ConstructMacro(VertexBuffer, m_VertexBuffer, this, bufferHandle);
			}

			bufferSize = GetIndexBufferSize();
			if (bufferSize != 0)
			{
				ResourceHandle bufferHandle;
				if (!GetDevice()->CreateBuffer(GPUBufferTypes::Index, bufferSize, bufferHandle).Wait())
					return;

				cb->CopyBuffer(GetHandle(), bufferHandle);

				m_IndexBuffer = RenderSystemAllocators::RenderSystemAllocator_Allocate<IndexBuffer>();
				ConstructMacro(IndexBuffer, m_IndexBuffer, this, bufferHandle);
			}

			if (!GetDevice()->SubmitCommandBuffer(&cb, 1).Wait())
				return;

			GetDevice()->DestroyCommandBuffer(&cb, 1).Wait();
		}
	}
}