// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Common\BitwiseUtils.h>
#include <Common\CharacterUtility.h>
#include <Debugging\Debug.h>
#include <fstream>
#include <map>
#include <stdarg.h>
#include <mutex>
#include <Windows.h>
#include <filesystem>

using namespace std::experimental::filesystem;

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		std::mutex m_Lock;
		std::map<uint32, FILE*> files;

		PlatformFile::Handle PushFile(FILE *FILE)
		{
			std::lock_guard<std::mutex> gaurd(m_Lock);

			static uint32 handleNumber = 0;

			files[++handleNumber] = FILE;

			return (PlatformFile::Handle)handleNumber;
		}

		FILE *GetFile(PlatformFile::Handle Handle)
		{
			if (files.find(Handle) != files.end())
				return files[(uint32)Handle];

			Assert(false, "The handle is invalid");
		}

		FILE *PullFile(PlatformFile::Handle Handle)
		{
			std::lock_guard<std::mutex> gaurd(m_Lock);

			FILE *file = GetFile(Handle);

			files.erase(Handle);

			return file;
		}

		template<typename T>
		const T *GetOpenModes(PlatformFile::OpenModes Mode)
		{
			//http://www.cplusplus.com/reference/cstdio/fopen/

			T * arguments = new T[11];
			uint8 index = 0;

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::OpenModes::Input))
				arguments[index++] = 'r';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::OpenModes::Output))
				arguments[index++] = 'w';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::OpenModes::Binary))
				arguments[index++] = 'b';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::OpenModes::Append))
			{
				arguments[index++] = 'a';
				arguments[index++] = '+';
			}

			arguments[index] = CharacterUtility::Character<T, '\0'>::Value;

			return arguments;
		}

		int GetSeekMode(PlatformFile::SeekModes Mode)
		{
			//http://www.cplusplus.com/reference/cstdio/fseek/

			switch (Mode)
			{
			case PlatformFile::SeekModes::Set:
				return SEEK_SET;

			case PlatformFile::SeekModes::Current:
				return SEEK_CUR;

			case PlatformFile::SeekModes::End:
				return SEEK_END;
			}
		}

		PlatformFile::Handle PlatformFile::Open(cwstr Path, OpenModes Mode)
		{
			FILE *file = nullptr;
			_wfopen_s(&file, Path, GetOpenModes<char16>(Mode));

			if (file == nullptr)
				return 0;

			return PushFile(file);
		}

		void PlatformFile::Close(Handle Handle)
		{
			FILE *file = PullFile(Handle);

			fclose(file);

			//delete file;
		}

		bool PlatformFile::Seek(Handle Handle, SeekModes Mode, uint64 Offset)
		{
			return (fseek(GetFile(Handle), Offset, GetSeekMode(Mode)) == 0);
		}

		uint64 PlatformFile::Tell(Handle Handle)
		{
			return ftell(GetFile(Handle));
		}

		void PlatformFile::Rewind(Handle Handle)
		{
			return rewind(GetFile(Handle));
		}

		uint64 PlatformFile::Size(Handle Handle)
		{
			Seek(Handle, SeekModes::End, 0);

			uint64 size = Tell(Handle);

			Rewind(Handle);

			return size;
		}

		uint8 PlatformFile::Read(Handle Handle, byte &Data)
		{
			return fread(&Data, sizeof(byte), 1, GetFile(Handle));
		}

		uint64 PlatformFile::Read(Handle Handle, byte *Data, uint64 Count)
		{
			return fread(Data, sizeof(byte), Count, GetFile(Handle));
		}

		uint8 PlatformFile::Read(Handle Handle, char8 &Data)
		{
			return fread(&Data, sizeof(char8), 1, GetFile(Handle));
		}

		uint64 PlatformFile::Read(Handle Handle, str Data, uint64 Count)
		{
			return fread(Data, sizeof(char8), Count, GetFile(Handle));
		}

		uint8 PlatformFile::Read(Handle Handle, char16 &Data)
		{
			return fread(&Data, sizeof(char16), 1, GetFile(Handle));
		}

		uint64 PlatformFile::Read(Handle Handle, wstr Data, uint64 Count)
		{
			return fread(Data, sizeof(char16), Count, GetFile(Handle));
		}

		//void PlatformFile::WriteFormatted(Handle Handle, cstr const Format, ...)
		//{
		//	//va_list args;
		//	//va_start(args, Format);
		//	//fprintf(GetFile(Handle), Format, args);
		//	//va_end(args);
		//}

		//void PlatformFile::WriteFormatted(Handle Handle, cwstr const Format, ...)
		//{
		//	//va_list args;
		//	//va_start(args, Format);
		//	//fwprintf(GetFile(Handle), Format, args);
		//	//va_end(args);
		//}

		void PlatformFile::Write(Handle Handle, byte Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}

		void PlatformFile::Write(Handle Handle, const byte *Data, uint32 Count)
		{
			fwrite(Data, sizeof(byte), Count, GetFile(Handle));
		}

		void PlatformFile::Write(Handle Handle, char8 Data)
		{
			fprintf(GetFile(Handle), "%c", Data);
		}
		void PlatformFile::Write(Handle Handle, cstr Data)
		{
			fprintf(GetFile(Handle), "%s", Data);
		}

		void PlatformFile::Write(Handle Handle, char16 Data)
		{
			fwprintf(GetFile(Handle), L"%c", Data);
		}
		void PlatformFile::Write(Handle Handle, cwstr Data)
		{
			fwprintf(GetFile(Handle), L"%s", Data);
		}

		void PlatformFile::Write(Handle Handle, int8 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, int16 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, int32 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, int64 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}

		void PlatformFile::Write(Handle Handle, uint16 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, uint32 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, uint64 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}

		void PlatformFile::Write(Handle Handle, float32 Data)
		{
			fprintf(GetFile(Handle), "%f", Data);
		}
		void PlatformFile::Write(Handle Handle, float64 Data)
		{
			fprintf(GetFile(Handle), "%f", Data);
		}
		void PlatformFile::Write(Handle Handle, float128 Data)
		{
			fprintf(GetFile(Handle), "%f", Data);
		}

		bool PlatformFile::Exists(cwstr Path)
		{
			return PlatformDirectory::Exists(Path);
		}

		bool PlatformFile::Delete(cwstr Path)
		{
			return PlatformDirectory::Delete(Path);
		}

		uint64 PlatformFile::GetLastWriteTime(cwstr Path)
		{
			return last_write_time(Path).time_since_epoch().count();
		}

		void PlatformFile::Move(cwstr SrceDirName, cwstr DestDirName)
		{
			return PlatformDirectory::Move(SrceDirName, DestDirName);
		}
	}
}
#endif