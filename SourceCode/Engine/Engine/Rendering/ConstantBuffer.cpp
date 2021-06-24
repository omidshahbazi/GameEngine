// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ConstantBuffer.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		ConstantBuffer::ConstantBuffer(ThreadedDevice* Device, uint32 Size, Handle Handle) :
			GPUBuffer(Device, Handle, Size, Types::Constant)
		{
		}

		void ConstantBuffer::Lock(Access Access)
		{
			GPUBuffer::Lock(Access);
		}

		void ConstantBuffer::Unlock(void)
		{
			GPUBuffer::Unlock();
		}

		void ConstantBuffer::Move(uint32 Count)
		{
			GPUBuffer::Move(Count);
		}

		void ConstantBuffer::Set(ConstantBuffer& Other)
		{
			Other.LockDirectly();
			Set(Other.Get<byte>(), GetSize());
			Other.UnlockDirectly();
		}

		void ConstantBuffer::Set(const byte* Data, uint16 Size)
		{
			PlatformMemory::Copy(Data, GetCurrentBuffer(), Mathematics::Min<uint16>(GetSize(), Size));
		}

		void ConstantBuffer::LockDirectly(Access Access)
		{
			GPUBuffer::Lock(Access, true);
		}

		void ConstantBuffer::UnlockDirectly(void)
		{
			GPUBuffer::Unlock(true);
		}

		ConstantBuffer* ConstantBuffer::Clone(void) const
		{
			ConstantBuffer* buffer = RenderingManager::GetInstance()->GetActiveDevice()->CreateConstantBuffer(GetSize());

			buffer->LockDirectly();
			buffer->Set(*buffer);
			buffer->UnlockDirectly();

			return buffer;
		}
	}
}