// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\ReferenceCountedInfo.h>

#ifndef REFERENCE_COUNTED_H
#define REFERENCE_COUNTED_H

namespace Engine
{
	using namespace Common;
	
	namespace MemoryManagement
	{
		class MEMORYMANAGERMENT_API ReferenceCounted : public ReferenceCountedInfo
		{
		public:
			virtual void Drop(void)
			{
				ReferenceCountedInfo::Drop();
			}

			//implement destroy as callback
		};
	}
}

#endif