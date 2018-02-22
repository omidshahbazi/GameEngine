// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <Common\PrimitiveTypes.h>

#ifndef BYTES_OF_H
#define BYTES_OF_H

namespace Engine
{
	namespace Common
	{
		template<typename T>
		union BytesOf
		{
		public:
			T Value;
			byte Bytes[sizeof(T)];

			static 	T GetMin(void)
			{
				BytesOf<T> value;

				for (uint8 i = 0; i < sizeof(T); ++i)
					value.Bytes[i] = 0;

				return value.Value;
			}

			static T GetMax(void)
			{
				BytesOf<T> value;

				for (uint8 i = 0; i < sizeof(T); ++i)
					value.Bytes[i] = 255;

				return value.Value;
			}
		};
	}
}

#endif