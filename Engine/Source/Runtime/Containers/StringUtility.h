// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H

#include <Containers\StringStream.h>
#include <stdarg.h>

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
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int16 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int32 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(int64 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint8 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint16 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint32 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(uint64 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(float32 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static DynamicString<T> ToString(float64 Value)
			{
				Buffer<T, false> stream;
				stream << Value;
				return DynamicString<T>(stream.GetBuffer(), stream.GetSize());
			}

			template<typename T>
			INLINE static bool ToBool(const DynamicString<T>& Value, bool DefaultValue = false)
			{
				return DefaultValue;
			}

			template<>
			INLINE static bool ToBool(const DynamicString<char8>& Value, bool DefaultValue)
			{
				if ((Value.StartsWith("True") || Value.StartsWith("Yes") || Value.StartsWith("1")))
					return true;
				else if ((Value.StartsWith("False") || Value.StartsWith("No") || Value.StartsWith("0")))
					return false;

				return DefaultValue;
			}

			template<>
			INLINE static bool ToBool(const DynamicString<char16>& Value, bool DefaultValue)
			{
				if ((Value.StartsWith(L"True") || Value.StartsWith(L"Yes") || Value.StartsWith(L"1")))
					return true;
				else if ((Value.StartsWith(L"False") || Value.StartsWith(L"No") || Value.StartsWith(L"0")))
					return false;

				return DefaultValue;
			}

			template<typename T>
			INLINE static int8 ToInt8(const DynamicString<T>& Value, int8 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static int8 ToInt8(const DynamicString<char8>& Value, int8 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static int8 ToInt8(const DynamicString<char16>& Value, int8 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int16 ToInt16(const DynamicString<T>& Value, int16 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static int16 ToInt16(const DynamicString<char8>& Value, int16 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static int16 ToInt16(const DynamicString<char16>& Value, int16 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int32 ToInt32(const DynamicString<T>& Value, int32 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static int32 ToInt32(const DynamicString<char8>& Value, int32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static int32 ToInt32(const DynamicString<char16>& Value, int32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static int64 ToInt64(const DynamicString<T>& Value, const int64& DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static int64 ToInt64(const DynamicString<char8>& Value, const int64& DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoll(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static int64 ToInt64(const DynamicString<char16>& Value, const int64& DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint8 ToUInt8(const DynamicString<T>& Value, uint8 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static uint8 ToUInt8(const DynamicString<char8>& Value, uint8 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static uint8 ToUInt8(const DynamicString<char16>& Value, uint8 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint16 ToUInt16(const DynamicString<T>& Value, uint16 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static uint16 ToUInt16(const DynamicString<char8>& Value, uint16 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoi(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static uint16 ToUInt16(const DynamicString<char16>& Value, uint16 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoi(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint32 ToUInt32(const DynamicString<T>& Value, uint32 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static uint32 ToUInt32(const DynamicString<char8>& Value, uint32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoll(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static uint32 ToUInt32(const DynamicString<char16>& Value, uint32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoll(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static uint64 ToUInt64(const DynamicString<T>& Value, const uint64& DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static uint64 ToUInt64(const DynamicString<char8>& Value, const uint64& DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atoll(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static uint64 ToUInt64(const DynamicString<char16>& Value, const uint64& DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atoll(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static float32 ToFloat32(const DynamicString<T>& Value, float32 DefaultValue = 0)
			{
				return DefaultValue;
			}

			template<>
			INLINE static float32 ToFloat32(const DynamicString<char8>& Value, float32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atof(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static float32 ToFloat32(const DynamicString<char16>& Value, float32 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atof(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			template<typename T>
			INLINE static float64 ToFloat64(const DynamicString<T>& Value, float64 DefaultValue = 0.f)
			{
				return DefaultValue;
			}

			template<>
			INLINE static float64 ToFloat64(const DynamicString<char8>& Value, float64 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char8>(Value.GetValue()))
					return std::atof(Value.GetValue());

				return DefaultValue;
			}

			template<>
			INLINE static float64 ToFloat64(const DynamicString<char16>& Value, float64 DefaultValue)
			{
				if (CharacterUtility::IsDigit<char16>(Value.GetValue()))
					return std::atof(Value.ChangeType<char8>().GetValue());

				return DefaultValue;
			}

			//https://stackoverflow.com/questions/16647278/minimal-implementation-of-sprintf-or-printf
			INLINE static uint32 my_vsprintf(char* buf, const char* fmt, va_list va)
			{
				char c;
				const char* save = buf;

				while ((c = *fmt++)) {
					int width = 0;
					enum flag_itoa flags = 0;
					if (c != '%') {
						*(buf++) = c;
						continue;
					}
				redo_spec:
					c = *fmt++;
					switch (c) {
					case '%':
						*(buf++) = c;
						break;
					case 'c':;
						*(buf++) = va_arg(va, int);
						break;
					case 'd':;
						int num = va_arg(va, int);
						if (num < 0) {
							num = -num;
							flags |= PUT_MINUS;
						}
						buf = sitoa(buf, num, width, flags | BASE_10);
						break;
					case 'u':
						buf = sitoa(buf, va_arg(va, unsigned int), width, flags | BASE_10);
						break;
					case 'x':
						buf = sitoa(buf, va_arg(va, unsigned int), width, flags);
						break;
					case 'b':
						buf = sitoa(buf, va_arg(va, unsigned int), width, flags | BASE_2);
						break;
					case 's':;
						const char* p = va_arg(va, const char*);
						if (p) {
							while (*p)
								*(buf++) = *(p++);
						}
						break;
					case 'm':;
						const uint8_t* m = va_arg(va, const uint8_t*);
						width = min(width, 64); // buffer limited to 256!
						if (m)
							for (;;) {
								buf = sitoa(buf, *(m++), 2, FILL_ZERO);
								if (--width <= 0)
									break;
								*(buf++) = ':';
							}
						break;
					case '0':
						if (!width)
							flags |= FILL_ZERO;
						// fall through
					case '1'...'9':
						width = width * 10 + c - '0';
						goto redo_spec;
					case '*':
						width = va_arg(va, unsigned int);
						goto redo_spec;
					case '+':
						flags |= PUT_PLUS;
						goto redo_spec;
					case '\0':
					default:
						*(buf++) = '?';
					}
					width = 0;
				}
				*buf = '\0';
				return buf - save;
			}

			INLINE static uint32 Format(str Result, cstr Content, va_list Args)
			{
				return vsprintf(Result, Content, Args);
			}

			INLINE static uint32 Format(wstr Result, cwstr Content, va_list Args)
			{
				return vswprintf(Result, Content, Args);
			}

			template<typename T>
			INLINE static void Format(DynamicString<T>& Result, const DynamicString<T> Content, ...)
			{
				va_list args;
				va_start(args, Content);
				Format(Result, Content, args);
				va_end(args);
			}

			template<typename T>
			INLINE static DynamicString<T> Format(const DynamicString<T> Content, ...)
			{
				va_list args;
				va_start(args, Content);
				DynamicString<T> result = Format(Content, args);
				va_end(args);

				return result;
			}

			INLINE static void Format(DynamicString<char8>& Result, const DynamicString<char8>& Content, va_list Args)
			{
				char8 content[2048] = {};
				Format(content, Content.GetValue(), Args);

				Result += content;
			}

			INLINE static void Format(DynamicString<char16>& Result, const DynamicString<char16>& Content, va_list Args)
			{
				char16 content[2048] = {};
				Format(content, Content.GetValue(), Args);

				Result += content;
			}

			INLINE static DynamicString<char8> Format(const DynamicString<char8>& Content, va_list Args)
			{
				DynamicString<char8> res;
				Format(res, Content, Args);
				return res;
			}

			INLINE static DynamicString<char16> Format(const DynamicString<char16>& Content, va_list Args)
			{
				DynamicString<char16> res;
				Format(res, Content, Args);
				return res;
			}
		};
	}
}

#endif