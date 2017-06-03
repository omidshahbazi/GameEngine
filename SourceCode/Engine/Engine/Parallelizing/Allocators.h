// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

#ifndef ALLOCATORS_H
#define ALLOCATORS_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		class PARALLELIZING_API Allocators
		{
		public:
			static DynamicSizeAllocator JobSystemAllocator;
			static DynamicSizeAllocator JobAllocator;
		};
	}
}

#endif