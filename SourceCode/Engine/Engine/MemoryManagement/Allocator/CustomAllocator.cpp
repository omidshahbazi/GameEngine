// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\CustomAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

#ifdef ONLY_USING_C_ALLOCATOR
#include <Platform\PlatformMemory.h>
#endif

#if DEBUG_MODE
#include <sstream>
#endif

namespace Engine
{
	using namespace Debugging;
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
#define IS_ADDRESS_IN_BOUND(Pointer) (ReinterpretCast(byte*, Pointer) >= m_StartAddress && ReinterpretCast(byte*, Pointer) < m_EndAddress)
#define CHECK_ADDRESS_BOUND(Pointer) Assert(IS_ADDRESS_IN_BOUND(Pointer), "Address doesn't belong to this allocator")

#ifdef DEBUG_MODE
			const int8 MEMORY_CORRUPTION_SIGN_SIZE = 8;
#endif

			CustomAllocator::CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
				AllocatorBase(Name),
				m_Parent(Parent),
				m_ReservedSize(ReserveSize),
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

				if (m_ReservedSize == 0)
					m_ReservedSize = m_Parent->GetReservedSize() * Initializer::GetInstance()->GetReserveSizeRate(Name);

				Assert(m_ReservedSize != 0, "m_ReservedSize must have a positive value");

				uint64 reserveSize = m_ReservedSize + GetHeaderSize();

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
#ifdef DEBUG_MODE
				return AllocateInternal(Size, File, LineNumber, Function);
#else
				return AllocateInternal(Size);
#endif
			}

#ifdef DEBUG_MODE
			byte* CustomAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* CustomAllocator::Reallocate(byte* Address, uint64 Size)
#endif
			{
				Assert(Size != 0, "Allocating zero size is not applicable");

#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Reallocate(Address, Size);
#else

				MemoryHeader* header = nullptr;

				if (Address != nullptr)
				{
					CHECK_ADDRESS_BOUND(Address);

					header = GetHeaderFromAddress(Address);

					if (header->Size >= Size)
					{
#ifdef DEBUG_MODE
						SetDebugInfo(header, File, LineNumber, Function);
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
				Assert(Address != nullptr, "Address cannot be null");

#ifdef ONLY_USING_C_ALLOCATOR
				Platform::PlatformMemory::Free(Address);
#else
				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader* header = GetHeaderFromAddress(Address);

				Deallocate(header);
#endif
			}

			bool CustomAllocator::TryDeallocate(byte* Address)
			{
				Assert(Address != nullptr, "Address cannot be null");

#ifdef ONLY_USING_C_ALLOCATOR
				Platform::PlatformMemory::Free(Address);
#else
				if (!IS_ADDRESS_IN_BOUND(Address))
					return false;

				MemoryHeader* header = GetHeaderFromAddress(Address);

				Deallocate(header);
#endif

				return true;
			}

#ifdef DEBUG_MODE
			void CustomAllocator::CheckForLeak(void)
			{
				std::stringstream stream;

				if (m_LastAllocatedHeader != nullptr)
				{
					MemoryHeader* header = m_LastAllocatedHeader;

					while (header != nullptr)
					{
						PrintMemoryInfo(stream, header);

						stream << "\n";

						header = header->Previous;
					}
				}

				Assert(m_LastAllocatedHeader == nullptr, "Memory leak detected in allocator [%s] -> %s", GetName(), stream.str().c_str());
			}
#endif

#ifdef DEBUG_MODE
			byte* CustomAllocator::AllocateInternal(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* CustomAllocator::AllocateInternal(uint64 Size)
#endif
			{
				Assert(Size != 0, "Allocating zero size is not applicable");
				Assert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Allocate(Size);
#else
				MemoryHeader* header = nullptr;

				if (m_LastFreeHeader != nullptr)
				{
					header = FindBestFitHeader(m_LastFreeHeader, Size);

					if (header != nullptr)
					{
						ReallocateHeader(header);

						Assert(m_ReservedSize >= m_TotalAllocated + header->Size, "Invalid m_TotalAllocated value");
						m_TotalAllocated += header->Size;
					}
				}

				if (header == nullptr)
				{
					byte* address = m_LastFreeAddress;
					m_LastFreeAddress += GetHeaderSize() + Size;

#ifdef DEBUG_MODE
					m_LastFreeAddress += MEMORY_CORRUPTION_SIGN_SIZE;
#endif

					Assert(m_LastFreeAddress <= m_EndAddress, "Not enough memory to allocate");

					address += GetHeaderSize();

					header = InitializeHeader(address, Size);

					Assert(m_ReservedSize >= m_TotalAllocated + Size, "Invalid m_TotalAllocated value");
					m_TotalAllocated += Size;
				}

#ifdef DEBUG_MODE
				SetDebugInfo(header, File, LineNumber, Function);
#endif

				if (m_LastAllocatedHeader != nullptr)
					m_LastAllocatedHeader->Next = header;

				header->Previous = m_LastAllocatedHeader;
				m_LastAllocatedHeader = header;

				CheckForCircularLink(m_LastAllocatedHeader);

				return GetAddressFromHeader(header);
#endif
			}

			void CustomAllocator::Deallocate(MemoryHeader* Header)
			{
#ifndef ONLY_USING_C_ALLOCATOR
#ifdef DEBUG_MODE
				if (!Header->IsAllocated)
				{
					std::stringstream stream;
					PrintMemoryInfo(stream, Header);

					Debug::AssertionFailed(DEBUG_ARGUMENTS, "!IsAllocated", "Memory already deallocated -> %s", stream.str().c_str());
				}

				Assert(Header->IsAllocated, "");
#endif

				FreeHeader(Header, m_LastFreeHeader);

				m_LastFreeHeader = Header;

				Assert(m_TotalAllocated >= Header->Size, "Invalid m_TotalAllocated value");
				m_TotalAllocated -= Header->Size;

#ifdef DEBUG_MODE
				PlatformSet(GetAddressFromHeader(Header), 0, Header->Size);
#endif
#endif
			}

			MemoryHeader* CustomAllocator::InitializeHeader(byte* Address, uint64 Size)
			{
				Assert(Address != nullptr, "Address cannot be null");

				CHECK_ADDRESS_BOUND(Address);

				MemoryHeader* header = GetHeaderFromAddress(Address);

				header->Size = Size;
				header->Previous = nullptr;
				header->Next = nullptr;

				return header;
			}

			void CustomAllocator::FreeHeader(MemoryHeader* Header, MemoryHeader* LastFreeHeader)
			{
				Assert(Header != nullptr, "Header cannot be null");

				CHECK_ADDRESS_BOUND(Header);

#ifdef DEBUG_MODE
				CheckCorruption(Header);

				Header->IsAllocated = false;

				RemoveHeaderFromList(Header);

				if (m_LastAllocatedHeader == Header)
					m_LastAllocatedHeader = Header->Previous;
#endif

				if (LastFreeHeader != nullptr)
					LastFreeHeader->Next = Header;

				Header->Previous = LastFreeHeader;

#ifdef DEBUG_MODE
				CheckForCircularLink(Header);

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

				RemoveHeaderFromList(Header);

				if (m_LastFreeHeader == Header)
					m_LastFreeHeader = Header->Previous;

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

			void CustomAllocator::RemoveHeaderFromList(MemoryHeader* Header)
			{
				if (Header->Previous != nullptr)
					Header->Previous->Next = Header->Next;

				if (Header->Next != nullptr)
					Header->Next->Previous = Header->Previous;

#ifdef DEBUG_MODE
				CheckForCircularLink(Header);
#endif
			}

#ifdef DEBUG_MODE
			void CustomAllocator::SetDebugInfo(MemoryHeader* Header, cstr File, uint32 LineNumber, cstr Function)
			{
				Header->IsAllocated = true;
				Header->File = File;
				Header->LineNumber = LineNumber;
				Header->Function = Function;

				byte* corruptionSign = GetAddressFromHeader(Header) + Header->Size;

				for (uint8 i = 0; i < MEMORY_CORRUPTION_SIGN_SIZE; ++i)
					corruptionSign[i] = i;
			}

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

				if (corrupted)
				{
					std::stringstream stream;
					PrintMemoryInfo(stream, Header);

					Debug::AssertionFailed(DEBUG_ARGUMENTS, "CheckCorruption()", "Memory corruption detected -> %s", stream.str().c_str());
				}
			}

			void CustomAllocator::CheckForCircularLink(MemoryHeader* Header)
			{
				MemoryHeader* currentHeader = Header;
				while (currentHeader != nullptr)
				{
					currentHeader = currentHeader->Previous;

					Assert(Header != currentHeader, "Circular link detected");
				}
			}
#endif

			void CustomAllocator::PrintMemoryInfo(std::stringstream& Stream, MemoryHeader* Header, uint8 ValueLimit)
			{
				byte* address = GetAddressFromHeader(Header);

				Stream << "Address: ";
				Stream << ReinterpretCast(void*, address);
				Stream << " Size: ";
				Stream << Header->Size;
				Stream << "b Allocated By: ";
				Stream << Header->Function;
				Stream << " File: ";
				Stream << Header->File;
				Stream << " Line: ";
				Stream << Header->LineNumber;
				Stream << " Allocator: [";
				Stream << GetName();
				Stream << "] Value: ";

				uint8 count = (Header->Size > ValueLimit ? ValueLimit : Header->Size);
				for (uint8 i = 0; i < count; ++i)
				{
					byte b = *(address + i);

					if (b == 0)
						continue;

					Stream << b;
				}

				if (count < Header->Size)
					Stream << "...";
			}
		}
	}
}