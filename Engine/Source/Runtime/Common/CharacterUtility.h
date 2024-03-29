// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CHARACTER_UTILITY_H
#define CHARACTER_UTILITY_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace Common
	{
		class CharacterUtility
		{
		public:
			template<typename T, T C>
			class Character
			{
			public:
				static constexpr const T Value = C;
			};

			template<typename T>
			static INLINE uint32 GetLength(const T* Value)
			{
				uint32 count = 0;

				if (Value != nullptr)
					while (Value[count] != Character<T, '\0'>::Value)
						count++;

				return count;
			}

			template<typename T>
			static INLINE bool AreEquals(const T* ValueA, const T* ValueB)
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
			static INLINE bool IsNewLine(T C)
			{
				return (C == Character<T, '\n'>::Value || C == Character<T, '\r'>::Value);
			}

			template<typename T>
			static INLINE bool IsEOL(T C)
			{
				return (IsNewLine(C) || C == Character<T, '\0'>::Value);
			}

			template<typename T>
			static INLINE bool IsWhitespace(T C)
			{
				return (C == Character<T, ' '>::Value || C == Character<T, '\t'>::Value || IsNewLine(C));
			}

			template<typename T>
			static INLINE bool IsDigit(T Value)
			{
				return (Character<T, '0'>::Value <= Value && Value <= Character<T, '9'>::Value);
			}

			template<typename T>
			static INLINE uint32 IsDigit(const T* const Value)
			{
				if (Value == nullptr)
					return false;

				uint32 index = 0;
				while (Value[index] != Character<T, '\0'>::Value)
				{
					T ch = Value[index++];
					if (!IsDigit(ch) && ch != Character<T, '.'>::Value && ch != Character<T, '-'>::Value)
						return false;
				}

				return true;
			}

			template<typename T>
			static INLINE bool IsLetter(T C)
			{
				return (Character<T, 'a'>::Value <= C && C <= Character<T, 'z'>::Value) || (Character<T, 'A'>::Value <= C && C <= Character<T, 'Z'>::Value);
			}

			template<typename T>
			static INLINE uint32 IsLetter(const T* const Value)
			{
				if (Value == nullptr)
					return false;

				uint32 index = 0;
				while (Value[index] != Character<T, '\0'>::Value)
				{
					T ch = Value[index++];
					if (!IsLetter(ch))
						return false;
				}

				return true;
			}

			template<typename T>
			static INLINE bool IsAlphabetic(T C)
			{
				return (IsLetter(C) || C == Character<T, '_'>::Value);
			}

			static INLINE bool IsAlphanumeric(char8 c)
			{
				return (IsAlphabetic(c) || IsDigit(c));
			}

			template<typename T>
			static INLINE T ToLower(T C)
			{
				if (Character<T, 'A'>::Value <= C && C <= Character<T, 'Z'>::Value)
					return (T)(C + 32);

				return C;
			}

			template<typename T>
			static INLINE T ToUpper(T C)
			{
				if (Character<T, 'a'>::Value <= C && C <= Character<T, 'z'>::Value)
					return (T)(C - 32);

				return C;
			}

			template<typename T, typename U>
			static INLINE void ChangeType(const T* const Value, U* NewValue)
			{
				uint32 length = GetLength(Value);

				ChangeType(Value, NewValue, length + 1);
			}

			template<typename T, typename U>
			static INLINE void ChangeType(const T* const Value, U* NewValue, uint32 Length)
			{
				for (uint32 i = 0; i < Length; ++i)
					NewValue[i] = Value[i];
			}

			template<typename T>
			static INLINE bool IsWideChar(T C)
			{
				return !(0 <= C && C <= 127);
			}

			template<typename T>
			static INLINE bool IsWideChar(const T* Value)
			{
				if (Value == nullptr)
					return false;

				uint32 index = 0;
				while (Value[index] != Character<T, '\0'>::Value)
				{
					T ch = Value[index++];
					if (!IsWideChar(ch))
						return false;
				}

				return true;
			}

			template<typename T>
			static INLINE bool ContainsAnyWideChar(const T* Value)
			{
				if (Value == nullptr)
					return false;

				uint32 index = 0;
				while (Value[index] != Character<T, '\0'>::Value)
				{
					T ch = Value[index++];
					if (IsWideChar(ch))
						return true;
				}

				return false;
			}
		};
	}
}

#endif