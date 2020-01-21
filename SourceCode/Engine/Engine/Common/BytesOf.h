// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BYTES_OF_H
#define BYTES_OF_H

#include <Common\PrimitiveTypes.h>

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

			static T GetMin(void)
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