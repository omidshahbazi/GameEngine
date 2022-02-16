// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\IndexBuffer.h>
#include <RenderSystem\SubMesh.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderCommon\Helper.h>

namespace Engine
{
	using namespace RenderCommon;

	namespace RenderSystem
	{
		IndexBuffer::IndexBuffer(SubMesh* SubMesh, ResourceHandle Handle) :
			GPUBuffer(SubMesh->GetDevice(), Handle, SubMesh->GetIndexBufferSize()),
			m_SubMesh(SubMesh)
		{
		}

		void IndexBuffer::Lock(GPUBufferAccess Access)
		{
			GPUBuffer::Lock(Access);
		}

		void IndexBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();

			if (GetLastLockAccess() == GPUBufferAccess::ReadOnly)
				return;

			CopyTo(m_SubMesh->GetIndexBufferSize());
		}

		void IndexBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(Helper::GetMeshIndexSize() * Count);
		}

		void IndexBuffer::Seek(uint32 Index)
		{
			GPUBuffer::Seek(Helper::GetMeshIndexSize() * Index);
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