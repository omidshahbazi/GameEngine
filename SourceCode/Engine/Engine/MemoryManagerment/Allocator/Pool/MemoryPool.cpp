// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\Pool\MemoryPool.h>
#include <Platform\Memory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Platform;
	using namespace Debugging;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			namespace Pool
			{
				const uint64 SIZE = 1073741824;

				MemoryPool::MemoryPool(void) :
					m_Memory(nullptr),
					m_LastFreeMemory(nullptr)
				{
					m_Memory = m_LastFreeMemory = Memory::Allocate(SIZE);
				}

				byte *MemoryPool::Get(uint32 Size)
				{
					Assert(m_LastFreeMemory != m_Memory + SIZE, "Pool has no more free memory");

					byte *memory = m_LastFreeMemory;

					m_LastFreeMemory += Size;

					return memory;
				}
			}
		}
	}
}