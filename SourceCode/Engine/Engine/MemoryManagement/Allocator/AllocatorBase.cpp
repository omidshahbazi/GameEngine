// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			AllocatorBase::AllocatorBase(cstr Name) :
				m_Name(Name)
			{
			}

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