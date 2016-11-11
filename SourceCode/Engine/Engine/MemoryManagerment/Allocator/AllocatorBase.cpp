// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>
#include <MemoryManagerment\Allocator\MemoryHeader.h>
#include <MemoryManagerment\Allocator\Pool\MemoryPool.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			void AllocatorBase::PlatformCopy(const byte *Source, byte *Destination, uint64 Size)
			{
				PlatformMemory::Copy(Source, Destination, Size);
			}

			void AllocatorBase::PlatformSet(byte *Address, int32 Value, uint64 Size)
			{
				PlatformMemory::Set(Address, Value, Size);
			}
		}
	}
}