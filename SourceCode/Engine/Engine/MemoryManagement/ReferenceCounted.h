// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef REFERENCE_COUNTED_H
#define REFERENCE_COUNTED_H

#include <MemoryManagement\ReferenceCountedInfo.h>

namespace Engine
{
	using namespace Common;
	
	namespace MemoryManagement
	{
		class MEMORYMANAGEMENT_API ReferenceCounted : public ReferenceCountedInfo
		{
		public:
			virtual void Drop(void) override
			{
				ReferenceCountedInfo::Drop();
			}

			//implement destroy as callback
		};
	}
}

#endif