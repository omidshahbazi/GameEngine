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
#define PRINT_NUMBER(CharType, PrintFunctionName, DataType) \
				CharType format##DataType[10] = {}; \
				uint32 writtenCount##DataType = 0; \
				writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "%%"); \
				if (forceHexSignOrDecimalPoint) \
					writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "#"); \
				if (forceSigned) \
					writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "+"); \
				if (fillZeros) \
					writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "0"); \
				if (isShorterSize) \
					for (uint8 i = 0; i < sizeSpecifier; ++i) \
						writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "h"); \
				if (isLongerSize) \
					for (uint8 i = 0; i < sizeSpecifier; ++i) \
						writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "l"); \
				writtenCount##DataType += sprintf(format##DataType + writtenCount##DataType, "%ii", width); \
				Result += PrintFunctionName(Result, format##DataType, va_arg(Args, DataType));

#define PUT_CHARACTER(Character) *(Result++) = Character

#define PRINT_STRING_POINTER(Pointer) \
				if (Pointer) \
					while (*Pointer) \
						*(Result++) = *(Pointer++);

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

			INLINE static uint32 Format(char8* Result, const char8* Content, va_list Args)
			{
				const char8* startOfResult = Result;

				char8 c;
				while ((c = *Content++) != '\0')
				{
					bool forceHexSignOrDecimalPoint = false;
					uint32 width = 0;
					bool fillZeros = false;
					bool forceSigned = false;
					uint8 sizeSpecifier = 0;
					bool isShorterSize = false;
					bool isLongerSize = false;
					bool isWString = false;

					if (c != '%')
					{
						PUT_CHARACTER(c);

						continue;
					}

				ContinueLoop:
					c = *Content++;

					switch (c)
					{
					case '%':
					{
						PUT_CHARACTER(c);
					}
					break;

					case 'i':
					case 'd':
					{
						PRINT_NUMBER(char8, sprintf, int64);
					}
					break;

					case 'u':
					{
						PRINT_NUMBER(char8, sprintf, uint64);
					}
					break;

					case 'x':
					{
						Result += sprintf(Result, "%x", va_arg(Args, uint32));
					}
					break;

					case 'X':
					{
						Result += sprintf(Result, "%X", va_arg(Args, uint32));
					}
					break;

					case 'f':
					{
						PRINT_NUMBER(char8, sprintf, float64);
					}
					break;

					case 'c':
					{
						PUT_CHARACTER(va_arg(Args, int32));
					}
					break;

					case 'p':
					{
						Result += sprintf(Result, "%p", va_arg(Args, void*));
					}
					break;

					case '0':
					{
						fillZeros = true;
						goto ContinueLoop;
					}

					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						width = (width * 10) + (c - '0');
						goto ContinueLoop;
					}

					case '#':
					{
						forceHexSignOrDecimalPoint = true;
						goto ContinueLoop;
					}

					case '*':
					{
						width = va_arg(Args, uint32);
						goto ContinueLoop;
					}

					case '+':
					{
						forceSigned = true;
						goto ContinueLoop;
					}

					case 'h':
					{
						++sizeSpecifier;
						isShorterSize = true;
						goto ContinueLoop;
					}

					case 'l':
					{
						++sizeSpecifier;
						isLongerSize = true;
						goto ContinueLoop;
					}

					case 'L':
					{
						++sizeSpecifier;
						isLongerSize = true;
						goto ContinueLoop;
					}

					case 's':
					{
						cstr p = va_arg(Args, cstr);
						PRINT_STRING_POINTER(p);
					}
					break;

					case 'S':
					{
						if (isWString)
						{
							cwstr wsp = va_arg(Args, WString).GetValue();
							PRINT_STRING_POINTER(wsp);
						}
						else
						{
							cstr sp = va_arg(Args, String).GetValue();
							PRINT_STRING_POINTER(sp);
						}
					}
					break;

					case 'W':
					{
						isWString = true;
						goto ContinueLoop;
					}

					case '\0':
					default:
					{
						PUT_CHARACTER('?');
					}
					break;
					}

					width = 0;
				}

				*Result = '\0';

				return Result - startOfResult;
			}

			INLINE static uint32 Format(char16* Result, const char16* Content, va_list Args)
			{
				const char16* startOfResult = Result;

				char16 c;
				while ((c = *Content++) != '\0')
				{
					bool forceHexSignOrDecimalPoint = false;
					uint32 width = 0;
					bool fillZeros = false;
					bool forceSigned = false;
					uint8 sizeSpecifier = 0;
					bool isShorterSize = false;
					bool isLongerSize = false;
					bool isWString = false;

					if (c != '%')
					{
						PUT_CHARACTER(c);

						continue;
					}

				ContinueLoop:
					c = *Content++;

					switch (c)
					{
					case '%':
					{
						PUT_CHARACTER(c);
					}
					break;

					case 'i':
					case 'd':
					{
						PRINT_NUMBER(char8, wprintf_s, int64);
					}
					break;

					case 'u':
					{
						PRINT_NUMBER(char8, wprintf_s, uint64);
					}
					break;

					case 'x':
					{
						Result += wprintf_s(Result, "%x", va_arg(Args, uint32));
					}
					break;

					case 'X':
					{
						Result += wprintf_s(Result, "%X", va_arg(Args, uint32));
					}
					break;

					case 'f':
					{
						PRINT_NUMBER(char8, wprintf_s, float64);
					}
					break;

					case 'c':
					{
						PUT_CHARACTER(va_arg(Args, int32));
					}
					break;

					case 'p':
					{
						Result += wprintf_s(Result, "%p", va_arg(Args, void*));
					}
					break;

					case '0':
					{
						fillZeros = true;
						goto ContinueLoop;
					}

					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						width = (width * 10) + (c - '0');
						goto ContinueLoop;
					}

					case '#':
					{
						forceHexSignOrDecimalPoint = true;
						goto ContinueLoop;
					}

					case '*':
					{
						width = va_arg(Args, uint32);
						goto ContinueLoop;
					}

					case '+':
					{
						forceSigned = true;
						goto ContinueLoop;
					}

					case 'h':
					{
						++sizeSpecifier;
						isShorterSize = true;
						goto ContinueLoop;
					}

					case 'l':
					{
						++sizeSpecifier;
						isLongerSize = true;
						goto ContinueLoop;
					}

					case 'L':
					{
						++sizeSpecifier;
						isLongerSize = true;
						goto ContinueLoop;
					}

					case 's':
					{
						cstr p = va_arg(Args, cstr);
						PRINT_STRING_POINTER(p);
					}
					break;

					case 'S':
					{
						if (isWString)
						{
							cwstr wsp = va_arg(Args, WString).GetValue();
							PRINT_STRING_POINTER(wsp);
						}
						else
						{
							cstr sp = va_arg(Args, String).GetValue();
							PRINT_STRING_POINTER(sp);
						}
					}
					break;

					case 'W':
					{
						isWString = true;
						goto ContinueLoop;
					}

					case '\0':
					default:
					{
						PUT_CHARACTER('?');
					}
					break;
					}

					width = 0;
				}

				*Result = '\0';

				return Result - startOfResult;
			}

			INLINE static uint32 Format(char8* Result, const char8* Content, ...)
			{
				va_list args;
				va_start(args, Content);
				uint32 len = Format(Result, Content, args);
				va_end(args);

				return len;
			}

			INLINE static uint32 Format(char16* Result, const char16* Content, ...)
			{
				va_list args;
				va_start(args, Content);
				uint32 len = Format(Result, Content, args);
				va_end(args);

				return len;
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

#undef PRINT_STRING_POINTER
#undef PUT_CHARACTER
#undef PRINT_NUMBER
	}
}

#endif