// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
#define MAX_PATH_LENGTH 260

		class PLATFORM_API PlatformFile
		{
		public:
			enum class OpenModes
			{
				Input = 2,
				Output = 4,
				Binary = 8,
				Append = 16,
				WideCharacter = 32
			};

			enum class SeekModes
			{
				Set = 0,
				Current,
				End
			};

			typedef uint32 Handle;

		public:
			static Handle Open(const char8 *Path, OpenModes Mode);
			static Handle Open(const char16 *Path, OpenModes Mode);

			static void Close(Handle Handle);

			static bool Seek(Handle Handle, SeekModes Mode, uint64 Offset);
			static uint64 Tell(Handle Handle);
			static void Rewind(Handle Handle);

			static uint64 Size(Handle Handle);

			static bool Read(Handle Handle, byte &Data);
			static bool Read(Handle Handle, byte *Data, uint64 Count);

			static bool Read(Handle Handle, char8 &Data);
			static bool Read(Handle Handle, str Data, uint64 Count);

			static bool Read(Handle Handle, char16 &Data);
			static bool Read(Handle Handle, wstr Data, uint64 Count);

			//static void WriteFormatted(Handle Handle, const char8 * const Format, ...);
			//static void WriteFormatted(Handle Handle, const char16 * const Format, ...);

			static void Write(Handle Handle, byte Data);
			static void Write(Handle Handle, const byte *Data, uint32 Count);

			static void Write(Handle Handle, char8 Data);
			static void Write(Handle Handle, cstr Data);

			static void Write(Handle Handle, char16 Data);
			static void Write(Handle Handle, cwstr Data);

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

			static void GetExecutingPath(str Path);
		};
	}
}

#endif