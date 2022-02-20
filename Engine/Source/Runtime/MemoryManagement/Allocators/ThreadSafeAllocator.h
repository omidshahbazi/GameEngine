// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_SAFE_ALLOCATOR_H
#define THREAD_SAFE_ALLOCATOR_H

#include <Common\ScopeGuard.h>

namespace Engine
{
	namespace Allocators
	{
		template<class AllocatorType>
		class ThreadSafeAllocator : public AllocatorType
		{
		public:
			template<typename... Parameters>
			ThreadSafeAllocator(Parameters... Arguments) :
				AllocatorType(std::forward<Parameters>(Arguments)...)
			{
			}

			virtual ~ThreadSafeAllocator(void)
			{
			}

#ifdef DEBUG_MODE
			byte* Allocate(uint64 Amount, cstr File, uint32 LineNumber, cstr Function) override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif

				return AllocatorType::Allocate(Amount, File, LineNumber, Function);
			}
#else
			byte* Allocate(uint64 Amount) override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif

				return AllocatorType::Allocate(Amount);
			}
#endif

#ifdef DEBUG_MODE
			byte* Reallocate(byte* Address, uint64 Amount, cstr File, uint32 LineNumber, cstr Function) override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif
				return AllocatorType::Reallocate(Address, Amount, File, LineNumber, Function);
			}
#else
			byte* Reallocate(byte* Address, uint64 Amount) override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif

				return AllocatorType::Reallocate(Address, Amount);
			}
#endif

			void Deallocate(byte* Address) override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif
				AllocatorType::Deallocate(Address);
			}

			bool TryDeallocate(byte* Address)  override
			{
#ifndef ONLY_USING_C_ALLOCATOR
				ScopeGuard gaurd(m_Lock);
#endif
				return AllocatorType::TryDeallocate(Address);
			}

		private:
#ifndef ONLY_USING_C_ALLOCATOR
			SpinLock m_Lock;
#endif
		};
	}
}

#endif