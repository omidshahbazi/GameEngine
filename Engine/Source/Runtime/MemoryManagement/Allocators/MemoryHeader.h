// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MEMORY_HEADER_H
#define MEMORY_HEADER_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Allocators
	{
		struct ALLOCATORS_API MemoryHeader
		{
		public:
			uint64 Size;
			MemoryHeader* Previous;
			MemoryHeader* Next;

#ifdef DEBUG_MODE
			bool IsAllocated;
			cstr File;
			uint32 LineNumber;
			cstr Function;
#endif
		};
	}
}

#endif