// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Program.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		ConstantBuffer::ConstantBuffer(Program* Program, uint32 Size, Handle Handle) :
			GPUBuffer((Program == nullptr ? nullptr : Program->GetDevice()), Handle, Size, Types::Constant)
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
			Other.Lock();
			Set(Other.Get<byte>(), GetSize());
			Other.Unlock();
		}

		void ConstantBuffer::Set(const byte* Data, uint16 Size)
		{
			PlatformMemory::Copy(Data, GetCurrentBuffer(), Mathematics::Min<uint16>(GetSize(), Size));
		}
	}
}