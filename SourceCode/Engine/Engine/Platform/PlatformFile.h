// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
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

			enum class Encodings
			{
				ASCII = 0,
				Unicode,
				UTF8,
				UTF16
			};

			enum class SeekModes
			{
				Set = 0,
				Current,
				End
			};

			enum class WatchNotifyFilter
			{
				FileRenamed = 2,
				DirectoryRenamed = 4,
				AttributeChanged = 8,
				FileSizeChanged = 16,
				LastWriteTimeChanged = 32,
				LastAccessTimeChanged = 64,
				CreationTimeChanged = 128,
				SecurtyAttributeChanged = 256
			};

			enum class WatchAction
			{
				Added = 0,
				Removed,
				Modified,
				RenamedOldName,
				RenamedNewName
			};

			typedef uint32 Handle;

			struct WatchInfo
			{
			public:
				WatchAction Action;
				cwstr FileName;
				uint32 FileNameLength;
			};

		public:
			static Handle Open(cwstr Path, OpenModes Mode, Encodings Encoding = Encodings::ASCII);

			static void Close(Handle Handle);

			static bool Seek(Handle Handle, SeekModes Mode, uint64 Offset);
			static uint64 Tell(Handle Handle);
			static void Rewind(Handle Handle);

			static uint64 Size(Handle Handle);

			static uint8 Read(Handle Handle, byte &Data);
			static uint64 Read(Handle Handle, byte *Data, uint64 Count);

			static uint8 Read(Handle Handle, char8 &Data);
			static uint64 Read(Handle Handle, str Data, uint64 Count);

			static uint8 Read(Handle Handle, char16 &Data);
			static uint64 Read(Handle Handle, wstr Data, uint64 Count);

			//static void WriteFormatted(Handle Handle, const char8 * const Format, ...);
			//static void WriteFormatted(Handle Handle, const char16 * const Format, ...);

			static void Write(Handle Handle, byte Data);
			static void Write(Handle Handle, const byte *Data, uint32 Count);

			static void Write(Handle Handle, char8 Data);
			static void Write(Handle Handle, cstr Data);
			static void Write(Handle Handle, cstr Data, uint32 Count);

			static void Write(Handle Handle, char16 Data);
			static void Write(Handle Handle, cwstr Data);
			static void Write(Handle Handle, cwstr Data, uint32 Count);

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

			static bool Exists(cwstr Path);

			static bool Delete(cwstr Path);

			static void Move(cwstr SrceDirName, cwstr DestDirName);

			static Handle CreateWatcher(cwstr Path, bool NonBlocking);
			static void CloseWatcher(Handle Handle);
			static void RefreshWatcher(Handle Handle, bool Recursive, WatchNotifyFilter Filters, WatchInfo *Infos, uint32 InfosLength, uint32 &InfosCount);

			static uint64 GetLastWriteTime(cwstr Path);
		};
	}
}

#endif