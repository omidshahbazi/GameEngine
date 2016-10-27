// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef DYNAMIC_SIZE_ALLOCATOR_H
#define DYNAMIC_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class DynamicSizeAllocator : public AllocatorBase
			{
			public:
				DynamicSizeAllocator(void);

			public:
				HandleInfo *Allocate(uint32 Size) override;

				void Deallocate(HandleInfo *Handle) override;

				void Update(void) override;

			private:
				HandleInfo *GetFirstHandle(void) const;

			private:
				HandleInfo *m_LastHandleInfo;
			};
		}
	}
}

#endif