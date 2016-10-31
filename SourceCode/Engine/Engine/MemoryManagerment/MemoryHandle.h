// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\ReferenceCounted.h>

#ifndef MEMORY_HANDLE_H
#define MEMORY_HANDLE_H

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

		class MemoryHandle : public ReferenceCountedInfo
		{
		public:
			MemoryHandle(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size);

			void Drop(void) override;

			void Delete(void);

			byte *Get(void) const
			{
				return Get<byte>();
			}

			template<typename T> T *Get(void) const
			{
				return (T*)m_Address;
			}

			uint32 GetSize(void) const
			{
				return m_Size;
			}

			AllocatorBase *GetOwnerAllocator(void) const
			{
				return m_OwnerAllocator;
			}

		private:
			AllocatorBase *m_OwnerAllocator;
			byte *m_Address;
			uint32 m_Size;
		};

		struct MemoryHandleExtra
		{
		public:
			MemoryHandleExtra *Previous;
			MemoryHandleExtra *Next;
			bool Referenced;
		};
	}
}

#endif