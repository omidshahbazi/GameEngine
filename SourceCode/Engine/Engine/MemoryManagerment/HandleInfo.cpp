// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\HandleInfo.h>
#include <MemoryManagerment\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		using namespace Allocator;

		HandleInfo::HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree) :
			m_OwnerAllocator(OwnerAllocator),
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(nullptr),
			Next(nullptr)
		{
		}

		HandleInfo::HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous) :
			m_OwnerAllocator(OwnerAllocator),
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(nullptr)
		{
		}

		HandleInfo::HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(Next)
		{
		}
	}
}