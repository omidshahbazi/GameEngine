// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Platform\PlatformMemory.h>
#include <memory>

namespace Engine
{
	namespace Platform
	{
		byte* PlatformMemory::Allocate(uint64 Size)
		{
			HardAssert(Size != 0, "Allocate 0 size is not applicable");

			byte* address = (byte*)malloc(Size);

			HardAssert(address != nullptr, "Allocation failed");

			return address;
		}

		byte* PlatformMemory::Reallocate(byte* Address, uint64 Size)
		{
			HardAssert(Size != 0, "Allocate 0 size is not applicable");

			byte* address = (byte*)realloc(Address, Size);

			HardAssert(address != nullptr, "Allocation failed");

			return address;
		}

		void PlatformMemory::Free(byte* Address)
		{
			HardAssert(Address != nullptr, "Deallocation of nullptr is not applicable");

			free(Address);
		}

		void PlatformMemory::Set(byte* Address, int32 Value, uint64 Size)
		{
			HardAssert(Address != nullptr, "Set to nullptr is not applicable");
			HardAssert(Size != 0, "Set 0 size is not applicable");

			memset(Address, Value, Size);
		}

		void PlatformMemory::Copy(const byte* Source, byte* Destination, uint64 Size)
		{
			HardAssert(Source != nullptr, "Copy from nullptr is not applicable");
			HardAssert(Destination != nullptr, "Copy to nullptr is not applicable");
			HardAssert(Size != 0, "Copy 0 size is not applicable");

			memcpy(Destination, Source, Size);
		}

		void PlatformMemory::Copy(const byte* Source, uint64 SourceIndex, byte* Destination, uint64 DestinationIndex, uint64 Size)
		{
			HardAssert(Source != nullptr, "Copy from nullptr is not applicable");
			HardAssert(Destination != nullptr, "Copy to nullptr is not applicable");
			HardAssert(Size != 0, "Copy 0 size is not applicable");

			memcpy(Destination + DestinationIndex, Source + SourceIndex, Size);
		}

		bool PlatformMemory::AreEqual(const byte* Left, const byte* Right, uint64 Length)
		{
			return AreEqual(Left, 0, Right, 0, Length);
		}

		bool PlatformMemory::AreEqual(const byte* Left, uint64 LeftIndex, const byte* Right, uint64 RightIndex, uint64 Length)
		{
			HardAssert(Left != nullptr, "Left cannot be null");
			HardAssert(Right != nullptr, "Right cannot be null");

			for (uint64 i = 0; i < Length; ++i)
				if (Left[LeftIndex + i] != Right[RightIndex + i])
					return false;

			return true;
		}
	}
}