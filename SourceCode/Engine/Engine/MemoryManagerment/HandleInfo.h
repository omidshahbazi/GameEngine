// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\ReferenceCounted.h>

#ifndef HANDLE_INFO
#define HANDLE_INFO

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			class AllocatorBase;
		}

		using namespace Allocator;

		class HandleInfo : public ReferenceCounted
		{
		public:
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree);
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous);
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next);

		private:
			AllocatorBase *m_OwnerAllocator;

		public:
			byte *Address;
			//std::atomic<void*> Address;
			uint32 Size;
			bool IsFree;
			HandleInfo *Previous;
			HandleInfo *Next;
		};
	}
}

#endif