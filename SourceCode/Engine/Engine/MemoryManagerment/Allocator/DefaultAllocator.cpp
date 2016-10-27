// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\HandleInfo.h>
#include <Platform\Memory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			HandleInfo *DefaultAllocator::Allocate(uint32 Size)
			{
				return new HandleInfo(this, Memory::Allocate(Size), Size, false);
			}

			void DefaultAllocator::Deallocate(HandleInfo *Handle)
			{
				Memory::Free(Handle->Address);

				delete Handle;
			}
		}
	}
}