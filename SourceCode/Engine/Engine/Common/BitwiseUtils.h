// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>

#ifndef BITWISE_UTILS_H
#define BITWISE_UTILS_H

namespace Engine
{
	namespace Common
	{
		class BitwiseUtils
		{
		public:
			template<typename T> static bool IsEnabled(T Mask, T Value)
			{
				return (((uint32)Mask & (uint32)Value) == (uint32)Value);
			}
		};
	}
}

#endif