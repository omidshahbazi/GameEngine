// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\MemoryHandle.h>
#include <MemoryManagerment\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		using namespace Allocator;

		MemoryHandle::MemoryHandle(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size) :
			m_OwnerAllocator(OwnerAllocator),
			m_Address(Address),
			m_Size(Size)
		{
		}

		void MemoryHandle::Drop(void)
		{
			ReferenceCountedInfo::Drop();

			if (GetCount() == 0)
				Delete();
		}

		void MemoryHandle::Delete(void)
		{
			m_OwnerAllocator->Deallocate(this);
		}
	}
}