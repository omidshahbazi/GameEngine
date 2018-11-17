// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H

#include <Containers\StringStream.h>

namespace Engine
{
	namespace Containers
	{
		class StringUtility
		{
		public:
			template<typename T>
			INLINE static DynamicString<T> ToString(bool Value, bool YesNo = false)
			{
				if (Value)
				{
					if (YesNo)
						return "Yes";
					else
						return "True";
				}
				else
				{
					if (YesNo)
						return "No";
					else
						return "False";
				}
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int8 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int16 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int32 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int64 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint8 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint16 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint32 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint64 &Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(float32 Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(float64 &Value)
			{
				Buffer<T> stream;
				stream << Value;
				return stream.GetBuffer();
			}

			template<typename T>
			INLINE static int32 ToInt32(const DynamicString<T> &Value)
			{
				return std::atoi(Value.GetValue());
			}

			template<typename T>
			INLINE static float32 ToFloat32(const DynamicString<T> &Value)
			{
				return std::atof(Value.GetValue());
			}
		};
	}
}

#endif