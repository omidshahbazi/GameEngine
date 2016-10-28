// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
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

		class HandleInfo : public ReferenceCountedInfo
		{
		public:
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree);
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous);
			HandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next);

			void Drop(void) override
			{
				ReferenceCountedInfo::Drop();

				if (GetCount() == 0)
					Delete();
			}

			void Delete(void);

			byte *Get(void) const
			{
				return Get<byte>();
			}

			template<typename T> T *Get(void) const
			{
				return (T*)Address;
			}

		private:
			AllocatorBase *m_OwnerAllocator;

		public:
			byte *Address;
			uint32 Size;
			bool IsFree;
			HandleInfo *Previous;
			HandleInfo *Next;
		};
	}
}

#endif