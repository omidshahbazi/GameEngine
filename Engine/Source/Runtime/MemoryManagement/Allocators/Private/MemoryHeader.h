// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ONLY_USING_C_ALLOCATOR
#ifndef MEMORY_HEADER_H
#define MEMORY_HEADER_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Allocators
	{
		namespace Private
		{
			struct ALLOCATORS_API MemoryHeader
			{
			public:
				uint64 Size;
				MemoryHeader* Next;

#ifdef DEBUG_MODE
				bool IsAllocated;
				uint64 UserSize;
				cstr File;
				uint32 LineNumber;
				cstr Function;
#endif
			};
		}
	}
}

#endif
#endif