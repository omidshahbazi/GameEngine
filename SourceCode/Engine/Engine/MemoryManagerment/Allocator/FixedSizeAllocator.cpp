// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			MemoryHandleExtra *GetExtraFromHandle(MemoryHandle *Handle)
			{
				return (MemoryHandleExtra*)((byte*)Handle + sizeof(MemoryHandle));
			}

			MemoryHandle *GetHandleFromExtra(MemoryHandleExtra *Extra)
			{
				return (MemoryHandle*)((byte*)Extra - sizeof(MemoryHandle));
			}

			FixedSizeAllocator::FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount) :
				m_BlockSize(BlockSize),
				m_BlockCount(BlockCount),
				m_Memory(nullptr),
				m_LastFreeIndex(0),
				m_FirstHandleExtra(nullptr),
				m_LastHandleExtra(nullptr)
			{
				m_Memory = GetFromPool(BlockCount * BlockSize);
			}

			MemoryHandle *FixedSizeAllocator::Allocate(uint32 Count)
			{
				MemoryHandle *handle = AllocateMemoryHandle(this, &m_Memory[m_LastFreeIndex * m_BlockSize], Count * m_BlockSize);

				MemoryHandleExtra *handleExtra = GetExtraFromHandle(handle);

				if (m_FirstHandleExtra == nullptr)
					m_FirstHandleExtra = handleExtra;

				handleExtra->Referenced = true;
				handleExtra->Next = nullptr;

				if (m_LastHandleExtra != nullptr)
				{
					m_LastHandleExtra->Next = handleExtra;
					handleExtra->Previous = m_LastHandleExtra;
				}
				else
					handleExtra->Previous = nullptr;

				m_LastHandleExtra = handleExtra;

				m_LastFreeIndex += Count;

				return handle;
			}

			void FixedSizeAllocator::Deallocate(MemoryHandle *Handle)
			{
				GetExtraFromHandle(Handle)->Referenced = false;
			}

			void FixedSizeAllocator::Defragment(void)
			{
				MemoryHandleExtra *extra = m_FirstHandleExtra;

				uint64 freeSize = 0;
				byte *freeAddress = nullptr;

				while (extra != nullptr)
				{
					if (extra->Referenced)
					{
						if (freeAddress == nullptr)
							goto GoToNext;
						else
						{
							MemoryHandle *handle = GetHandleFromExtra(extra);

							uint64 usedSize = 0;
							while (extra != nullptr)
							{
								if (!extra->Referenced)
									break;

								usedSize += GetHandleFromExtra(extra)->GetSize();

								extra = extra->Next;
							}

							// freeSize == usedSize
							// freeSize < usedSize
							// freeSize > usedSize

							//PlatformCopy(handle->Get(), freeAddress, )

							freeSize = 0;
							freeAddress = nullptr;

							continue;
						}
					}

					MemoryHandle *handle = GetHandleFromExtra(extra);

					freeSize += handle->GetSize();
					if (freeAddress == nullptr)
						freeAddress = handle->Get();

					DeallocateMemoryHandle(handle);

				GoToNext:
					extra = extra->Next;
				}
			}
		}
	}
}