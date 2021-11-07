// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\CustomAllocator.h>
#include <Allocators\Private\MemoryHeader.h>
#include <Platform\PlatformMemory.h>

#ifdef ONLY_USING_C_ALLOCATOR
#include <Allocators\DefaultAllocator.h>
#endif

namespace Engine
{
	using namespace Platform;

	namespace Allocators
	{
		using namespace Private;

#define IS_ADDRESS_IN_BOUND(Pointer) (ReinterpretCast(byte*, Pointer) >= m_StartAddress && ReinterpretCast(byte*, Pointer) < m_EndAddress)
#define CHECK_ADDRESS_BOUND(Pointer) HardAssert(IS_ADDRESS_IN_BOUND(Pointer), "Address doesn't belong to this allocator")

#ifdef CORRUPTED_HEAP_DETECTION
		const int8 HEAP_CORRUPTION_SIGN_SIZE = 8;
#endif

		CustomAllocator::CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
			AllocatorBase(Name)
			, m_Parent(Parent)

#ifndef ONLY_USING_C_ALLOCATOR
			, m_ReservedSize(ReserveSize)
			, m_StartAddress(nullptr)
			, m_EndAddress(nullptr)
			, m_LastFreeAddress(nullptr)
			, m_FirstFreeHeader(nullptr)
			, m_TotalAllocated(0)
#endif

#ifdef LEAK_DETECTION
			, m_FirstAllocatedHeader(nullptr)
#endif
		{
			HardAssert(m_Parent != nullptr, "Parent cannot be null");
			HardAssert(m_Parent != this, "Parent cannot be same as the allocator");

#ifndef ONLY_USING_C_ALLOCATOR
			HardAssert(m_ReservedSize != 0, "m_ReservedSize must have a positive value");

			uint64 reserveSize = m_ReservedSize + GetHeaderSize();

			m_StartAddress = m_LastFreeAddress = AllocateMemory(m_Parent, reserveSize);
			m_EndAddress = m_StartAddress + reserveSize;
#endif
		}

		CustomAllocator::~CustomAllocator(void)
		{
#ifdef LEAK_DETECTION
			CheckLeakage();
#endif

#ifndef ONLY_USING_C_ALLOCATOR
			m_Parent->Deallocate(m_StartAddress);
#endif
		}

#ifdef DEBUG_MODE
		byte* CustomAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* CustomAllocator::Reallocate(byte* Address, uint64 Size)
#endif
		{
			HardAssert(Size != 0, "Allocating zero size is not applicable");

#ifdef ONLY_USING_C_ALLOCATOR
#ifdef DEBUG_MODE
			return m_Parent->Reallocate(Address, Size, File, LineNumber, Function);
#else
			return m_Parent->Reallocate(Address, Size);
#endif
#else

			MemoryHeader* header = nullptr;

			if (Address != nullptr)
			{
				CHECK_ADDRESS_BOUND(Address);

				header = GetHeaderFromAddress(Address);

				if (header->Size >= Size)
				{
#ifdef DEBUG_MODE
					SetDebugInfo(header, Size, File, LineNumber, Function);
#endif

					return Address;
				}
			}

#ifdef DEBUG_MODE
			byte* newAddress = AllocateInternal(Size, File, LineNumber, Function);
#else
			byte* newAddress = AllocateInternal(Size);
#endif

			if (header != nullptr)
			{
				PlatformMemory::Copy(Address, newAddress, header->Size);

				Deallocate(header);
			}

			return newAddress;
#endif
		}

		void CustomAllocator::Deallocate(byte* Address)
		{
			HardAssert(Address != nullptr, "Address cannot be null");

#ifdef ONLY_USING_C_ALLOCATOR
			return DefaultAllocator::GetInstance()->Deallocate(Address);
#else
			CHECK_ADDRESS_BOUND(Address);

			Deallocate(GetHeaderFromAddress(Address));
#endif
		}

		bool CustomAllocator::TryDeallocate(byte* Address)
		{
			HardAssert(Address != nullptr, "Address cannot be null");

#ifdef ONLY_USING_C_ALLOCATOR
			return DefaultAllocator::GetInstance()->TryDeallocate(Address);
#else
			if (!IS_ADDRESS_IN_BOUND(Address))
				return false;

			Deallocate(GetHeaderFromAddress(Address));
#endif

			return true;
		}

#ifdef DEBUG_MODE
		byte* CustomAllocator::AllocateInternal(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* CustomAllocator::AllocateInternal(uint64 Size)
#endif
		{
			HardAssert(Size != 0, "Allocating zero size is not applicable");

#ifndef ONLY_USING_C_ALLOCATOR
			HardAssert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");
#endif

#ifdef ONLY_USING_C_ALLOCATOR
#ifdef DEBUG_MODE
			return m_Parent->Allocate(Size, File, LineNumber, Function);
#else
			return m_Parent->Allocate(Size);
#endif
#else
			MemoryHeader* header = nullptr;

			if (m_FirstFreeHeader != nullptr)
			{
				header = FindBestFitHeader(m_FirstFreeHeader, Size);

				if (header != nullptr)
				{
#ifdef DEBUG_MODE
					if (header->IsAllocated)
					{
						std::stringstream stream;
						stream << "Memory already allocated -> ";
						PrintMemoryInfo(stream, header);

						HardAssert(false, stream.str().c_str());
					}
#endif

					RemoveHeaderFromChain(header, &m_FirstFreeHeader);

					HardAssert(m_ReservedSize >= m_TotalAllocated + header->Size, "Invalid m_TotalAllocated value");
					m_TotalAllocated += header->Size;
				}
			}

			if (header == nullptr)
			{
				byte* address = m_LastFreeAddress;
				m_LastFreeAddress += GetHeaderSize() + Size;

#ifdef CORRUPTED_HEAP_DETECTION
				m_LastFreeAddress += HEAP_CORRUPTION_SIGN_SIZE;
#endif

				HardAssert(m_LastFreeAddress <= m_EndAddress, "Not enough memory to allocate");

				address += GetHeaderSize();

				header = InitializeHeader(address, Size);

				HardAssert(m_ReservedSize >= m_TotalAllocated + Size, "Invalid m_TotalAllocated value");
				m_TotalAllocated += Size;
			}

#ifdef DEBUG_MODE
			SetDebugInfo(header, Size, File, LineNumber, Function);
#endif

#ifdef LEAK_DETECTION
			AddHeaderToChain(header, &m_FirstAllocatedHeader);
#endif

			return GetAddressFromHeader(header);
#endif
		}

#ifndef ONLY_USING_C_ALLOCATOR
		void CustomAllocator::Deallocate(MemoryHeader* Header)
		{
#ifdef DEBUG_MODE
			if (!Header->IsAllocated)
			{
				std::stringstream stream;
				stream << "Memory already deallocated -> ";
				PrintMemoryInfo(stream, Header);

				HardAssert(false, stream.str().c_str());
			}
#endif

			FreeHeader(Header);

			HardAssert(m_TotalAllocated >= Header->Size, "Invalid m_TotalAllocated value");
			m_TotalAllocated -= Header->Size;

#ifdef DEBUG_MODE
			PlatformMemory::Set(GetAddressFromHeader(Header), 0, Header->UserSize);
#endif
		}

		MemoryHeader* CustomAllocator::InitializeHeader(byte* Address, uint64 Size)
		{
			HardAssert(Address != nullptr, "Address cannot be null");

			MemoryHeader* header = GetHeaderFromAddress(Address);

#ifdef DEBUG_MODE
			PlatformMemory::Set(header, 0, 1);
#endif

			header->Size = Size;
			header->Next = nullptr;

			return header;
		}

		void CustomAllocator::FreeHeader(MemoryHeader* Header)
		{
			HardAssert(Header != nullptr, "Header cannot be null");

#ifdef DEBUG_MODE
			Header->IsAllocated = false;
#endif

#ifdef CORRUPTED_HEAP_DETECTION
			CheckCorruption(Header);
#endif

#ifdef LEAK_DETECTION
			RemoveHeaderFromChain(Header, &m_FirstAllocatedHeader);
#endif

			AddHeaderToChain(Header, &m_FirstFreeHeader);
		}

		MemoryHeader* CustomAllocator::GetHeaderFromAddress(byte* Address)
		{
			HardAssert(Address != nullptr, "Address cannot be null");

			return ReinterpretCast(MemoryHeader*, Address - GetHeaderSize());
		}

		byte* CustomAllocator::GetAddressFromHeader(MemoryHeader* Header)
		{
			HardAssert(Header != nullptr, "Header cannot be null");

			return ((byte*)Header + GetHeaderSize());
		}

		uint32 CustomAllocator::GetHeaderSize(void)
		{
			return sizeof(MemoryHeader);
		}

		void CustomAllocator::AddHeaderToChain(MemoryHeader* Header, MemoryHeader** FirstHeader)
		{
			Header->Next = *FirstHeader;
			*FirstHeader = Header;
		}

		void CustomAllocator::RemoveHeaderFromChain(MemoryHeader* Header, MemoryHeader** FirstHeader)
		{
			if (Header == *FirstHeader)
			{
				*FirstHeader = Header->Next;
				return;
			}

			MemoryHeader* prevHeader = *FirstHeader;
			MemoryHeader* currentHeader = *FirstHeader;

			while ((currentHeader = currentHeader->Next) != nullptr)
			{
				if (currentHeader != Header)
				{
					prevHeader = currentHeader;

					continue;
				}

				prevHeader->Next = currentHeader->Next;
				break;
			}
		}

#ifdef DEBUG_MODE
		void CustomAllocator::SetDebugInfo(MemoryHeader* Header, uint64 UserSize, cstr File, uint32 LineNumber, cstr Function)
		{
			Header->IsAllocated = true;
			Header->UserSize = UserSize;
			Header->File = File;
			Header->LineNumber = LineNumber;
			Header->Function = Function;

#ifdef CORRUPTED_HEAP_DETECTION
			byte* corruptionSign = GetAddressFromHeader(Header) + Header->UserSize;

			for (uint8 i = 0; i < HEAP_CORRUPTION_SIGN_SIZE; ++i)
				corruptionSign[i] = i;
#endif
		}
#endif

#ifdef LEAK_DETECTION
		void CustomAllocator::CheckLeakage(void)
		{
			if (m_FirstAllocatedHeader == nullptr)
				return;

			std::stringstream stream;
			stream << "Memory leak detected in allocator ";

			MemoryHeader* header = m_FirstAllocatedHeader;

			while (header != nullptr)
			{
				stream << "\n";

				PrintMemoryInfo(stream, header);

				header = header->Next;
			}

			HardAssert(false, stream.str().c_str());
		}
#endif

#ifdef CORRUPTED_HEAP_DETECTION
		void CustomAllocator::CheckCorruption(MemoryHeader* Header)
		{
			byte* corruptionSign = GetAddressFromHeader(Header) + Header->UserSize;
			bool corrupted = false;
			for (uint8 i = 0; i < HEAP_CORRUPTION_SIGN_SIZE; ++i)
				if (corruptionSign[i] != i)
				{
					corrupted = true;
					break;
				}

			if (corrupted)
			{
				std::stringstream stream;
				stream << "CheckCorruption()", "Memory corruption detected -> ";
				PrintMemoryInfo(stream, Header);

				HardAssert(DEBUG_ARGUMENTS, stream.str().c_str());
			}
		}
#endif

#ifdef DEBUG_MODE
		void CustomAllocator::PrintMemoryInfo(std::stringstream& Stream, MemoryHeader* Header, uint8 ValueLimit)
		{
			byte* address = GetAddressFromHeader(Header);

			Stream << "Address: ";
			Stream << ReinterpretCast(void*, address);
			Stream << " Size: ";
			Stream << Header->UserSize;
			Stream << "b Allocated By: ";
			Stream << Header->Function;
			Stream << " File: ";
			Stream << Header->File;
			Stream << " Line: ";
			Stream << Header->LineNumber;

			Stream << " Allocator: [";
			Stream << GetName();
			Stream << "] Value: ";

			uint8 count = (Header->UserSize > ValueLimit ? ValueLimit : Header->UserSize);
			for (uint8 i = 0; i < count; ++i)
			{
				byte b = *(address + i);

				if (b == 0)
					continue;

				Stream << b;
			}

			if (count < Header->UserSize)
				Stream << "...";
		}
#endif

		void CustomAllocator::Reset(void)
		{
#ifndef ONLY_USING_C_ALLOCATOR
			m_LastFreeAddress = m_StartAddress;
			m_FirstFreeHeader = nullptr;
			m_TotalAllocated = 0;
#endif

#ifdef LEAK_DETECTION
			m_FirstAllocatedHeader = nullptr;
#endif
		}
#endif
	}
}