// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\CustomAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Debugging\Debug.h>
#include <Containers\Strings.h>

#ifdef ONLY_USING_C_ALLOCATOR
#include <Platform\PlatformMemory.h>
#endif

#if DEBUG_MODE
#include <iostream>
#endif

namespace Engine
{
	using namespace Debugging;
	using namespace Containers;

	namespace MemoryManagement
	{
		namespace Allocator
		{
#ifdef DEBUG_MODE
#define CHECK_ADDRESS_BOUND(Pointer) Assert(ReinterpretCast(byte*, Pointer) >= m_StartAddress && ReinterpretCast(byte*, Pointer) < m_EndAddress, "Address doesn't belong to this allocator")
#else
#define CHECK_ADDRESS_BOUND(Address)
#endif

#ifdef DEBUG_MODE
			const int8 MEMORY_CORRUPTION_SIGN_SIZE = 8;
#endif

			CustomAllocator::CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
				AllocatorBase(Name),
				m_Parent(Parent),
				m_ReserveSize(ReserveSize),
				m_StartAddress(nullptr),
				m_EndAddress(nullptr),
				m_LastFreeAddress(nullptr),
				m_LastFreeHeader(nullptr)
#ifndef ONLY_USING_C_ALLOCATOR
				, m_TotalAllocated(0)
#endif
#ifdef DEBUG_MODE
				, m_LastAllocatedHeader(nullptr)
#endif
			{
#ifndef ONLY_USING_C_ALLOCATOR
				Assert(m_Parent != nullptr, "Parent cannot be null");
				Assert(m_Parent != this, "Parent cannot be same as the allocator");

				uint32 reserveSize = m_ReserveSize + GetHeaderSize();
#ifdef DEBUG_MODE
				reserveSize += MEMORY_CORRUPTION_SIGN_SIZE;
#endif

				m_StartAddress = m_LastFreeAddress = AllocateMemory(m_Parent, reserveSize);
				m_EndAddress = m_StartAddress + reserveSize;
#endif
			}

			CustomAllocator::~CustomAllocator(void)
			{
#ifndef ONLY_USING_C_ALLOCATOR
#ifdef DEBUG_MODE
#ifdef LAEK_DETECTION
				CheckForLeak();
#endif
#endif
				m_Parent->Deallocate(m_StartAddress);
#endif
			}

#ifdef DEBUG_MODE
			byte* CustomAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* CustomAllocator::Allocate(uint64 Size)
#endif
			{
#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Allocate(Size);
#else
				Assert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

				if (m_LastFreeHeader != nullptr)
				{
					MemoryHeader* bestFitHeader = FindBestFitHeader(m_LastFreeHeader, Size);

					if (bestFitHeader != nullptr)
					{
						if (bestFitHeader == m_LastFreeHeader)
							m_LastFreeHeader = m_LastFreeHeader->Previous;

						ReallocateHeader(bestFitHeader);

						Assert(m_ReserveSize >= m_TotalAllocated + bestFitHeader->Size, "Invalid m_TotalAllocated value");
						m_TotalAllocated += bestFitHeader->Size;

						return GetAddressFromHeader(bestFitHeader);
					}
				}

				byte* address = m_LastFreeAddress;
				m_LastFreeAddress += GetHeaderSize() + Size;

#ifdef DEBUG_MODE
				m_LastFreeAddress += MEMORY_CORRUPTION_SIGN_SIZE;
#endif

				Assert(m_LastFreeAddress <= m_EndAddress, "End of the block is out of allocator's bound");

				address += GetHeaderSize();

#ifdef DEBUG_MODE
				InitializeHeader(address, Size, File, LineNumber, Function);
#else
				InitializeHeader(address, Size);

#endif
				Assert(m_ReserveSize >= m_TotalAllocated + Size, "Invalid m_TotalAllocated value");
				m_TotalAllocated += Size;

				return address;
#endif
			}

			void CustomAllocator::Deallocate(byte* Address)
			{
				Assert(Address != nullptr, "Address cannot be null");

#ifdef ONLY_USING_C_ALLOCATOR
				Platform::PlatformMemory::Free(Address);
#else
				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader* header = GetHeaderFromAddress(Address);

#ifdef DEBUG_MODE
				Assert(header->IsAllocated, "Memory already deallocated");
#endif

				FreeHeader(header, m_LastFreeHeader);

				m_LastFreeHeader = header;

				Assert(m_TotalAllocated >= header->Size, "Invalid m_TotalAllocated value");
				m_TotalAllocated -= header->Size;

#ifdef DEBUG_MODE
				PlatformSet(Address, 0, header->Size);
#endif
#endif
			}

#ifdef DEBUG_MODE
			void CustomAllocator::InitializeHeader(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			void CustomAllocator::InitializeHeader(byte* Address, uint64 Size)
#endif
			{
				Assert(Address != nullptr, "Address cannot be null");

				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader* header = GetHeaderFromAddress(Address);

				header->Size = Size;
				header->Next = header->Previous = nullptr;

#ifdef DEBUG_MODE
				header->IsAllocated = true;
				header->File = File;
				header->LineNumber = LineNumber;
				header->Function = Function;

				if (m_LastAllocatedHeader != nullptr)
					m_LastAllocatedHeader->Next = header;

				header->Previous = m_LastAllocatedHeader;
				m_LastAllocatedHeader = header;

				byte* corruptionSign = Address + Size;

				for (uint8 i = 0; i < MEMORY_CORRUPTION_SIGN_SIZE; ++i)
					corruptionSign[i] = i;
#endif
			}

			void CustomAllocator::FreeHeader(MemoryHeader* Header, MemoryHeader* LastFreeHeader)
			{
				Assert(Header != nullptr, "Header cannot be null");

				CHECK_ADDRESS_BOUND(Header);

#ifdef DEBUG_MODE
				CheckCorruption(Header);

				CheckForDuplicate(Header, LastFreeHeader);

				Header->IsAllocated = false;

				if (Header->Next != nullptr)
					Header->Next->Previous = Header->Previous;
				if (Header->Previous != nullptr)
					Header->Previous->Next = Header->Next;

				if (m_LastAllocatedHeader == Header)
					m_LastAllocatedHeader = Header->Previous;
#endif

				if (LastFreeHeader != nullptr)
					LastFreeHeader->Next = Header;

				Header->Previous = LastFreeHeader;

#ifdef DEBUG_MODE
				Header->Next = nullptr;
#endif
			}

			void CustomAllocator::ReallocateHeader(MemoryHeader* Header)
			{
				Assert(Header != nullptr, "Header cannot be null");
				CHECK_ADDRESS_BOUND(Header);

#ifdef DEBUG_MODE
				Assert(!Header->IsAllocated, "Memory already allocated");

				Header->IsAllocated = true;
#endif

				if (Header->Previous != nullptr)
					Header->Previous->Next = Header->Next;

				if (Header->Next != nullptr)
					Header->Next->Previous = Header->Previous;

				Header->Previous = nullptr;
				Header->Next = nullptr;
			}

			MemoryHeader* CustomAllocator::GetHeaderFromAddress(byte* Address)
			{
				Assert(Address != nullptr, "Address cannot be null");

				return ReinterpretCast(MemoryHeader*, Address - GetHeaderSize());
			}

			byte* CustomAllocator::GetAddressFromHeader(MemoryHeader* Header)
			{
				Assert(Header != nullptr, "Header cannot be null");

				return ((byte*)Header + GetHeaderSize());
			}

			uint32 CustomAllocator::GetHeaderSize(void)
			{
				return sizeof(MemoryHeader);
			}

#ifdef DEBUG_MODE
			void CustomAllocator::CheckCorruption(MemoryHeader* Header)
			{
				Assert(Header != nullptr, "Header cannot be null");

				byte* corruptionSign = GetAddressFromHeader(Header) + Header->Size;
				bool corrupted = false;
				for (uint8 i = 0; i < MEMORY_CORRUPTION_SIGN_SIZE; ++i)
					if (corruptionSign[i] != i)
					{
						corrupted = true;
						break;
					}

				Assert(!corrupted, "Memory corruption detected");
			}

			void CustomAllocator::CheckForDuplicate(MemoryHeader* Header, MemoryHeader* LastFreeHeader)
			{
				Assert(Header != nullptr, "Header cannot be null");

				MemoryHeader* header = LastFreeHeader;
				while (header != nullptr)
				{
					Assert(header != Header, "Going to add duplicate header in free list");

					header = header->Previous;
				}
			}

			void CustomAllocator::CheckForLeak(void)
			{
				if (m_LastAllocatedHeader != nullptr)
				{
					MemoryHeader* header = m_LastAllocatedHeader;

					while (header != nullptr)
					{
						std::cout << "Memory " << ReinterpretCast(void*, GetAddressFromHeader(header)) << " with size " << header->Size << "b allocated by [" << header->Function << "@" << header->File << ":Ln " << header->LineNumber << "] in allocator [" << GetName() << "]" << std::endl;

						header = header->Previous;
					}
				}

				Assert(m_LastAllocatedHeader == nullptr, "Memory leak occurs");
			}
#endif
		}
	}
}