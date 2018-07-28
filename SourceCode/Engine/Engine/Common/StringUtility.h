// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace Common
	{
		class StringUtility
		{
		public:
			template<typename T, T C>
			class Character
			{
			public:
				static const T Value = C;
			};

			template<typename T>
			static uint32 GetLength(const T *Value)
			{
				uint32 count = 0;

				if (Value != nullptr)
					while (Value[count] != Character<T, '\0'>::Value)
						count++;

				return count;
			}

			template<typename T>
			static bool AreEquals(const T *ValueA, const T *ValueB)
			{
				if (ValueA == ValueB)
					return true;

				uint32 length = GetLength(ValueA);

				if (length != GetLength(ValueB))
					return false;

				for (uint32 i = 0; i < length; ++i)
					if (ValueA[i] != ValueB[i])
						return false;

				return true;
			}

			template<typename T>
			static bool IsDigit(T C)
			{
				return (C >= Character<T, '0'>::Value && C <= Character<T, '9'>::Value);
			}

			template<typename T>
			static uint32 IsDigit(const T *Value)
			{
				if (Value == nullptr)
					return false;

				uint32 index = 0;
				while (Value[index] != Character<T, '\0'>::Value)
					if (!IsDigit(Value[index++]))
						return false;

				return true;
			}

			template<typename T>
			static bool IsNewLine(T C)
			{
				return (C == Character<T, '\n'>::Value || C == Character<T, '\r'>::Value);
			}

			template<typename T>
			static bool IsLetter(T C)
			{
				return (C >= Character<T, 'a'>::Value && C <= Character<T, 'z'>::Value) || (C >= Character<T, 'A'>::Value && C <= Character<T, 'Z'>::Value);
			}
		};
	}
}

#endif