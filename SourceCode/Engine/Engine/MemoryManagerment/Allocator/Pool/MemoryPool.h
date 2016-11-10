// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			namespace Pool
			{
				class MEMORYMANAGERMENT_API MemoryPool
				{
				public:
					MemoryPool(void);

				public:
					byte *Get(uint64 Size);

				private:
					byte *m_StartMemory;
					byte *m_EndMemory;
					byte *m_LastFreeMemory;
				};
			}
		}
	}
}

#endif