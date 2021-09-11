// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\VertexBuffer.h>
#include <RenderSystem\SubMesh.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderCommon\Helper.h>

namespace Engine
{
	using namespace RenderCommon;

	namespace RenderSystem
	{
		VertexBuffer::VertexBuffer(SubMesh* SubMesh, ResourceHandle Handle) :
			GPUBuffer(SubMesh->GetDevice(), Handle, SubMesh->GetVertexBufferSize(), GPUBufferTypes::Vertex),
			m_SubMesh(SubMesh)
		{
		}

		void VertexBuffer::Lock(GPUBufferAccess Access)
		{
			GPUBuffer::Lock(Access);
		}

		void VertexBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();

			if (GetLastLockAccess() == GPUBufferAccess::ReadOnly)
				return;

			GetDevice()->CopyFromBufferToVertex(GetHandle(), m_SubMesh->GetHandle(), m_SubMesh->GetVertexBufferSize());
		}

		void VertexBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(Helper::GetMeshVertexSize() * Count);
		}

		Vertex& VertexBuffer::Get(void)
		{
			return *ReinterpretCast(Vertex*, GetCurrentBuffer());
		}

		const Vertex& VertexBuffer::Get(void) const
		{
			return *ReinterpretCast(Vertex*, GetCurrentBuffer());
		}
	}
}