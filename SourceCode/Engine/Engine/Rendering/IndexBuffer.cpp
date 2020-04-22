// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\IndexBuffer.h>
#include <Rendering\SubMesh.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		IndexBuffer::IndexBuffer(SubMesh* SubMesh, Handle Handle) :
			GPUBuffer(SubMesh->GetDevice(), Handle, SubMesh->GetIndexBufferSize(), Types::PixelUnpack),
			m_SubMesh(SubMesh)
		{
		}

		void IndexBuffer::Lock(Access Access)
		{
			GPUBuffer::Lock(Access);
		}

		void IndexBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();
		}

		void IndexBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(m_SubMesh->GetIndexSize() * Count);
		}

		uint32& IndexBuffer::Get(void)
		{
			return *ReinterpretCast(uint32*, GetCurrentBuffer());
		}

		const uint32& IndexBuffer::Get(void) const
		{
			return *ReinterpretCast(uint32*, GetCurrentBuffer());
		}
	}
}