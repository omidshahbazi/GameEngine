// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

namespace Engine
{
	namespace Common
	{
		class StringUtils
		{
		public:
			template<typename T, T C> class Character
			{
			public:
				static const T Value = C;
			};

			template<typename T> static uint32 GetLength(const T *Value)
			{
				uint32 count = 0;

				if (Value != nullptr)
					while (Value[count] != Character<T, '\0'>::Value)
						count++;

				return count;
			}
		};
	}
}

#endif