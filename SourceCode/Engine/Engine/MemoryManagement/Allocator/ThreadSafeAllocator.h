// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_SAFE_ALLOCATOR_H
#define THREAD_SAFE_ALLOCATOR_H

#include <mutex>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			template<class AllocatorType>
			class ThreadSafeAllocator : public AllocatorType
			{
			public:
				ThreadSafeAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize = 0) :
					AllocatorType(Name, Parent, ReserveSize)
				{
				}

				virtual ~ThreadSafeAllocator(void)
				{
				}

#ifdef DEBUG_MODE
				byte* Allocate(uint64 Amount, cstr File, uint32 LineNumber, cstr Function) override
				{
					std::lock_guard<std::mutex> gaurd(m_Lock);

					return AllocatorType::Allocate(Amount, File, LineNumber, Function);
				}
#else
				byte* Allocate(uint64 Amount) override
				{
					std::lock_guard<std::mutex> gaurd(m_Lock);

					return AllocatorType::Allocate(Amount);
				}
#endif

				void Deallocate(byte* Address) override
				{
					std::lock_guard<std::mutex> gaurd(m_Lock);

					AllocatorType::Deallocate(Address);
				}

				bool TryDeallocate(byte* Address)  override
				{
					std::lock_guard<std::mutex> gaurd(m_Lock);

					return AllocatorType::TryDeallocate(Address);
				}
				
			private:
				std::mutex m_Lock;
			};
		}
	}
}

#endif