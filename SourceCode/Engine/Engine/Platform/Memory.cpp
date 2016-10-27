// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\Memory.h>
#include <Debugging\Debug.h>
#include <memory>

namespace Engine
{
	using namespace Debugging;

	namespace Platform
	{
		byte *Memory::Allocate(uint32 Size)
		{
			Assert(Size != 0, "Allocate 0 size is not applicable");

			byte *address = (byte*)malloc(Size);

			Assert(address != nullptr, "Allocation failed");

			return address;
		}

		void Memory::Free(byte *Address)
		{
			Assert(Address != nullptr, "Deallocation of nullptr is not applicable");

			free(Address);
		}

		void Memory::Set(byte *Address, int32 Value, uint32 Size)
		{
			Assert(Address != nullptr, "Set to nullptr is not applicable");
			Assert(Size != 0, "Set 0 size is not applicable");

			memset(Address, Value, Size);
		}

		void Memory::Copy(const byte *Source, byte *Destination, uint32 Size)
		{
			Assert(Source != nullptr, "Copy from nullptr is not applicable");
			Assert(Destination != nullptr, "Copy to nullptr is not applicable");
			Assert(Size != 0, "Copy 0 size is not applicable");

			memcpy(Destination, Source, Size);
		}
	}
}