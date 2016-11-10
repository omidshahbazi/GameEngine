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
				//const uint64 SIZE = 6442450944;
				const uint64 SIZE = 1024;

				MemoryPool::MemoryPool(void) :
					m_StartMemory(nullptr),
					m_EndMemory(nullptr),
					m_LastFreeMemory(nullptr)
				{
					m_StartMemory = m_LastFreeMemory = Memory::Allocate(SIZE);
					m_EndMemory = m_StartMemory + SIZE;
				}

				byte *MemoryPool::Get(uint64 Size)
				{
					Assert(m_LastFreeMemory < m_EndMemory, "Pool has no more free memory");

					byte *memory = m_LastFreeMemory;

					m_LastFreeMemory += Size;

					Assert(m_LastFreeMemory <= m_EndMemory, "End of the block is out of pool's bound");

					return memory;
				}
			}
		}
	}
}