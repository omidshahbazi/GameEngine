// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\VertexBuffer.h>
#include <Rendering\SubMesh.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		VertexBuffer::VertexBuffer(SubMesh* SubMesh, Handle Handle) :
			GPUBuffer(SubMesh->GetDevice(), Handle, SubMesh->GetVertexBufferSize(), Types::Vertex),
			m_SubMesh(SubMesh)
		{
		}

		void VertexBuffer::Lock(Access Access)
		{
			GPUBuffer::Lock(Access);
		}

		void VertexBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();

			if (GetLastLockAccess() == Access::ReadOnly)
				return;

			GetDevice()->CopyFromBufferToVertex(GetHandle(), m_SubMesh->GetHandle(), m_SubMesh->GetVertexBufferSize());
		}

		void VertexBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(m_SubMesh->GetVertexSize() * Count);
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