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

		private:
			AllocatorBase *m_OwnerAllocator;
			byte *m_Address;
			uint32 m_Size;
		};
	}
}

#endif