// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\CustomAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Debugging\Debug.h>
#include <sstream>

//#ifdef ONLY_USING_C_ALLOCATOR
#include <Platform\PlatformMemory.h>
//#endif

namespace Engine
{
	using namespace Debugging;

	namespace MemoryManagement
	{
		namespace Allocator
		{

#if DEBUG_MODE
#define CHECK_ADDRESS_BOUND(Pointer) Assert(reinterpret_cast<byte*>(Pointer) >= m_StartAddress && reinterpret_cast<byte*>(Pointer) < m_EndAddress, "Address doesn't belong to this allocator")
#else
#define CHECK_ADDRESS_BOUND(Address)
#endif

			CustomAllocator::CustomAllocator(cstr Name, AllocatorBase *Parent, uint64 ReserveSize) :
				AllocatorBase(Name),
				m_Parent(Parent),
				m_ReserveSize(ReserveSize),
				m_StartAddress(nullptr),
				m_EndAddress(nullptr),
				m_LastFreeAddress(nullptr),
				m_LastFreeHeader(nullptr)
#if DEBUG_MODE
				, m_LastAllocatedHeader(nullptr)
#endif
			{
#ifndef ONLY_USING_C_ALLOCATOR
				Assert(m_Parent != nullptr, "Parent cannot be null");
				Assert(m_Parent != this, "Parent cannot be same as the allocator");

				m_StartAddress = m_LastFreeAddress = AllocateMemory(m_Parent, m_ReserveSize);
				m_EndAddress = m_StartAddress + m_ReserveSize;
#endif
			}

			CustomAllocator::~CustomAllocator(void)
			{
#ifndef ONLY_USING_C_ALLOCATOR
#if DEBUG_MODE
				if (m_LastAllocatedHeader != nullptr)
				{
					MemoryHeader *header = m_LastAllocatedHeader;

					while (header != nullptr)
					{
						std::stringstream ss;

						ss << "Memory " << reinterpret_cast<void*>(GetAddressFromHeader(header)) << " with size " << header->Size << "b allocated by [" << header->Function << "@" << header->File << ":Ln " << header->LineNumber << "] in allocator [" << GetName() << "]" << std::endl;

						Debug::Print(ss.str().c_str());

						header = header->Previous;
					}
				}

				Assert(m_LastAllocatedHeader == nullptr, "Memory leak occurs");
#endif

				m_Parent->Deallocate(m_StartAddress);
#endif
			}

#if DEBUG_MODE
			byte *CustomAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte *CustomAllocator::Allocate(uint64 Size)
#endif
			{
#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Allocate(Size);
#else
				Assert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

				byte *address = nullptr;

				if (m_LastFreeHeader != nullptr)
				{
					address = GetFromFreeList(m_LastFreeHeader, Size);

					if (address != nullptr)
					{
						if (GetHeaderFromAddress(address) == m_LastFreeHeader)
							m_LastFreeHeader = m_LastFreeHeader->Previous;

						return address;
					}
				}

				address = m_LastFreeAddress;
				m_LastFreeAddress += GetHeaderSize() + Size;

				Assert(m_LastFreeAddress <= m_EndAddress, "End of the block is out of allocator's bound");

				address += GetHeaderSize();

#if DEBUG_MODE
				InitializeHeader(address, Size, File, LineNumber, Function);
#else
				InitializeHeader(address, Size);

#endif

				return address;
#endif
			}

			void CustomAllocator::Deallocate(byte *Address)
			{
#ifdef ONLY_USING_C_ALLOCATOR
				Platform::PlatformMemory::Free(Address);
#else
				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader *header = GetHeaderFromAddress(Address);

				FreeHeader(header, m_LastFreeHeader);

				m_LastFreeHeader = header;

#if DEBUG_MODE
				PlatformSet(Address, 0, header->Size);
#endif
#endif
			}

#if DEBUG_MODE
			void CustomAllocator::InitializeHeader(byte *Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			void CustomAllocator::InitializeHeader(byte *Address, uint64 Size)
#endif
			{
				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader *header = GetHeaderFromAddress(Address);

				header->Size = Size;
				header->Next = header->Previous = nullptr;

#if DEBUG_MODE
				header->File = File;
				header->LineNumber = LineNumber;
				header->Function = Function;

				if (m_LastAllocatedHeader != nullptr)
					m_LastAllocatedHeader->Next = header;

				header->Previous = m_LastAllocatedHeader;
				m_LastAllocatedHeader = header;
#endif
			}

			void CustomAllocator::FreeHeader(MemoryHeader *Header, MemoryHeader *LastFreeHeader)
			{
				CHECK_ADDRESS_BOUND(Header);

#if DEBUG_MODE
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

#if DEBUG_MODE
				Header->Next = nullptr;
#endif
			}

			void CustomAllocator::ReallocateHeader(MemoryHeader *Header)
			{
				CHECK_ADDRESS_BOUND(Header);

				Assert(Header != nullptr, "Header cannot be null");

				if (Header->Previous != nullptr)
					Header->Previous->Next = Header->Next;

				if (Header->Next != nullptr)
					Header->Next->Previous = Header->Previous;
			}

			MemoryHeader *CustomAllocator::GetHeaderFromAddress(byte *Address)
			{
				return reinterpret_cast<MemoryHeader*>(Address - GetHeaderSize());
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