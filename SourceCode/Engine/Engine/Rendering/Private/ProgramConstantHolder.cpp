// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramConstantHolder.h>
#include <Rendering\ConstantBuffer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Hash.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			ProgramConstantHolder::ConstantHash ProgramConstantHolder::GetHash(const String& Name)
			{
				return Hash::CRC32(Name.GetValue(), Name.GetLength());
			}

			bool ProgramConstantHolder::SetBuffer(ConstantHash Hash, const byte* Data, uint16 Size)
			{
				ConstantBuffer* buffer = GetConstantBuffer(Hash);
				if (buffer == nullptr)
					return false;

				buffer->Lock();
				buffer->Set(Data, Size);
				buffer->Unlock();

				return true;
			}
		}
	}
}