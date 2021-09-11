// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JSON_BASIC_H
#define JSON_BASIC_H

#include <JSON\JSONData.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class AllocatorBase;
		}
	}

	using namespace MemoryManagement::Allocator;

	namespace JSON
	{
		class JSON_API JSONBasic
		{
		public:
			virtual ~JSONBasic(void)
			{
			}

			virtual String ToString(void) const = 0;

		protected:
			static String ToString(const JSONData* Data);
		};
	}
}

#endif