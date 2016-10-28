// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class DefaultAllocator : public AllocatorBase
			{
			public:
				MemoryHandle *Allocate(uint32 Size);

				void Deallocate(MemoryHandle *Handle) override;

				static DefaultAllocator &GetInstance(void)
				{
					if (instance == nullptr)
						instance = new DefaultAllocator();

					return *instance;
				}

			private:
				static DefaultAllocator *instance;
			};
		}
	}
}

#endif