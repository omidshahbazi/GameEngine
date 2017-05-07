// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformFile
		{
		public:
			enum class Modes
			{
				Input = 2,
				Output = 4,
				Binary = 8,
				Append = 10,
				WideCharacter = 12
			};

			typedef uint32 Handle;

		public:
			static Handle Open(const char8 *Path, Modes Mode);
			static Handle Open(const char16 *Path, Modes Mode);

			static void Close(Handle Handle);

			static void WriteFormatted(Handle Handle, const char8 * const Format, ...);
			static void WriteFormatted(Handle Handle, const char16 * const Format, ...);

			static void Write(Handle Handle, byte Data);
			static void Write(Handle Handle, const byte *Data, uint32 Count);

			static void Write(Handle Handle, char8 Data);
			static void Write(Handle Handle, const char8 *Data);

			static void Write(Handle Handle, char16 Data);
			static void Write(Handle Handle, const char16 *Data);

			static void Write(Handle Handle, int8 Data);
			static void Write(Handle Handle, int16 Data);
			static void Write(Handle Handle, int32 Data);
			static void Write(Handle Handle, int64 Data);

			static void Write(Handle Handle, uint16 Data);
			static void Write(Handle Handle, uint32 Data);
			static void Write(Handle Handle, uint64 Data);

			static void Write(Handle Handle, float32 Data);
			static void Write(Handle Handle, float64 Data);
			static void Write(Handle Handle, float128 Data);
		};
	}
}

#endif