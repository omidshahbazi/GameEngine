// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\CustomAllocator.h>
#include <MemoryManagerment\MemoryHeader.h>
#include <MemoryManagerment\Allocator\Pool\MemoryPool.h>
#include <Platform\Memory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			using namespace Pool;

			MemoryPool MEMORY_POOL;

			CustomAllocator::CustomAllocator(uint64 ReserveSize) :
				m_ReserveSize(ReserveSize),
				m_StartAddress(nullptr),
				m_EndAddress(nullptr),
				m_LastFreeAddress(nullptr),
				m_LastFreeHeader(nullptr)
			{
				m_StartAddress = m_LastFreeAddress = MEMORY_POOL.Get(m_ReserveSize);
				m_EndAddress = m_StartAddress + m_ReserveSize;
			}

			byte *CustomAllocator::AllocateInternal(uint64 Size)
			{
				byte *address = nullptr;

				if (m_LastFreeHeader != nullptr)
				{
					address = GetFromFreeList(m_LastFreeHeader, Size);

					if (address != nullptr)
					{
						m_LastFreeHeader = m_LastFreeHeader->Previous;
						return address;
					}
				}

				Assert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

				address = m_LastFreeAddress;
				m_LastFreeAddress += GetHeaderSize() + Size;

				InitializeHeader(address, Size);

				return address;
			}

			void CustomAllocator::DeallocateInternal(byte *Address)
			{
				MemoryHeader *header = GetHeaderFromAddress(Address);

				FreeHeader(header, m_LastFreeHeader);

				m_LastFreeHeader = header;
			}

			void CustomAllocator::InitializeHeader(byte *Address, uint64 Size)
			{
				MemoryHeader *header = GetHeaderFromAddress(Address);

				header->Size = Size;
				header->Next = header->Previous = nullptr;
			}

			void CustomAllocator::FreeHeader(MemoryHeader *Header, MemoryHeader *LastFreeHeader)
			{
				Header->Previous = LastFreeHeader;

				if (LastFreeHeader != nullptr)
					LastFreeHeader->Next = Header;
			}

			void CustomAllocator::ReallocateHeader(MemoryHeader *Header)
			{
				Assert(Header != nullptr, "Header cannot be null");

				if (Header->Previous != nullptr)
					Header->Previous->Next = Header->Next;

				if (Header->Next != nullptr)
					Header->Next->Previous = Header->Previous;
			}

			MemoryHeader *CustomAllocator::GetHeaderFromAddress(byte *Address)
			{
				return (MemoryHeader*)(Address - GetHeaderSize());
			}

			byte *CustomAllocator::GetAddressFromHeader(MemoryHeader *Extra)
			{
				return ((byte*)Extra + GetHeaderSize());
			}

			uint32 CustomAllocator::GetHeaderSize(void)
			{
				return sizeof(MemoryHeader);
			}
		}
	}
}