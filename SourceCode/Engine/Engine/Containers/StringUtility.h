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
			INLINE static bool ToBool(const DynamicString<T> &Value, bool DefaultValue = false)
			{
				if ((Value.StartsWith("True") || Value.StartsWith("Yes") || Value.StartsWith("1")))
					return true;
				else if ((Value.StartsWith("False") || Value.StartsWith("No") || Value.StartsWith("0")))
					return false;

				return DefaultValue;
			}

			template<typename T>
			INLINE static int8 ToInt8(const DynamicString<T> &Value, int8 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int16 ToInt16(const DynamicString<T> &Value, int16 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int32 ToInt32(const DynamicString<T> &Value, int32 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int64 ToInt64(const DynamicString<T> &Value, const int64 &DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint8 ToUInt8(const DynamicString<T> &Value, uint8 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint16 ToUInt16(const DynamicString<T> &Value, uint16 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint32 ToUInt32(const DynamicString<T> &Value, uint32 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint64 ToUInt64(const DynamicString<T> &Value, const uint64 &DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atoll(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static float32 ToFloat32(const DynamicString<T> &Value, float32 DefaultValue = 0)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atof(Value.GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static float64 ToFloat64(const DynamicString<T> &Value, float64 DefaultValue = 0.f)
			{
				if (CharacterUtility::IsDigit<T>(Value.GetValue()))
					return std::atof(Value.GetValue());

				return DefaultValue;
			}
		};
	}
}

#endif