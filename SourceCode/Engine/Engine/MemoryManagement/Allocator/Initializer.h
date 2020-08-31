// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <MemoryManagement\Allocator\DefaultAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API Initializer
			{
				CREATOR_DECLARATION(Initializer);

			private:
				Initializer(void);
			};
		}
	}
}

#endif