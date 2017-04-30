// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\PlatformFile.h>
#include <Common\BitwiseUtils.h>
#include <Common\StringUtils.h>
#include <fstream>
#include <map>
#include <stdarg.h>

using namespace std;

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		std::map<uint32, FILE*> files;

		PlatformFile::Handle PushFile(FILE *FILE)
		{
			static uint32 handleNumber = 0;

			files[++handleNumber] = FILE;

			return (PlatformFile::Handle)handleNumber;
		}

		FILE *GetFile(PlatformFile::Handle Handle)
		{
			return files[(uint32)Handle];
		}

		FILE *PullFile(PlatformFile::Handle Handle)
		{
			FILE *file = GetFile(Handle);

			files.erase(Handle);

			return file;
		}

		template<typename T> const T *GetArguments(PlatformFile::Modes Mode)
		{
			//http://www.cplusplus.com/reference/cstdio/fopen/

			T * arguments = new T[11];
			uint8 index = 0;

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::Modes::Input))
				arguments[index++] = 'r';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::Modes::Output))
				arguments[index++] = 'w';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::Modes::Binary))
				arguments[index++] = 'b';

			if (BitwiseUtils::IsEnabled(Mode, PlatformFile::Modes::Append))
				arguments[index++] = 'a+';

			arguments[index] = StringUtils::Character<T, '\0'>::Value;

			return arguments;
		}

		PlatformFile::Handle PlatformFile::Open(const char8 *Path, Modes Mode)
		{
			FILE *file = nullptr;
			fopen_s(&file, Path, GetArguments<char8>(Mode));

			if (file == nullptr)
				return 0;

			return PushFile(file);
		}

		PlatformFile::Handle PlatformFile::Open(const char16 *Path, Modes Mode)
		{
			FILE *file = nullptr;
			_wfopen_s(&file, Path, GetArguments<char16>(Mode));

			if (file == nullptr)
				return 0;

			return PushFile(file);
		}

		void PlatformFile::Close(Handle Handle)
		{
			FILE *file = PullFile(Handle);

			fclose(file);

			delete file;
		}

		void PlatformFile::WriteFormatted(Handle Handle, const char8 * const Format, ...)
		{
			va_list args;
			va_start(args, Format);
			fprintf(GetFile(Handle), Format, args);
			va_end(args);
		}

		void PlatformFile::WriteFormatted(Handle Handle, const char16 * const Format, ...)
		{
			va_list args;
			va_start(args, Format);
			fwprintf(GetFile(Handle), Format, args);
			va_end(args);
		}

		void PlatformFile::Write(Handle Handle, byte Data)
		{
			WriteFormatted(Handle, "i", Data);
		}

		void PlatformFile::Write(Handle Handle, const byte *Data, uint32 Count)
		{
			fwrite(Data, sizeof(byte), Count, GetFile(Handle));
		}

		void PlatformFile::Write(Handle Handle, char8 Data)
		{
			WriteFormatted(Handle, "c", Data);
		}
		void PlatformFile::Write(Handle Handle, const char8 *Data)
		{
			WriteFormatted(Handle, "s", Data);
		}

		void PlatformFile::Write(Handle Handle, char16 Data)
		{
			WriteFormatted(Handle, "c", Data);
		}
		void PlatformFile::Write(Handle Handle, const char16 *Data)
		{
			WriteFormatted(Handle, "s", Data);
		}

		void PlatformFile::Write(Handle Handle, int8 Data)
		{
			WriteFormatted(Handle, "i", Data);
		}
		void PlatformFile::Write(Handle Handle, int16 Data)
		{
			//WriteFormatted(Handle, "i", Data);???? fprintf(GetFile(Handle), % ??
		}
		void PlatformFile::Write(Handle Handle, int32 Data)
		{
			fprintf(GetFile(Handle), "%i", Data);
		}
		void PlatformFile::Write(Handle Handle, int64 Data)
		{
			WriteFormatted(Handle, "i", Data);
		}

		void PlatformFile::Write(Handle Handle, uint16 Data)
		{
			WriteFormatted(Handle, "i", Data);
		}
		void PlatformFile::Write(Handle Handle, uint32 Data)
		{
			WriteFormatted(Handle, "i", Data);
		}
		void PlatformFile::Write(Handle Handle, uint64 Data)
		{
			WriteFormatted(Handle, "i", Data);
		}

		void PlatformFile::Write(Handle Handle, float32 Data)
		{
			WriteFormatted(Handle, "f", Data);
		}
		void PlatformFile::Write(Handle Handle, float64 Data)
		{
			WriteFormatted(Handle, "f", Data);
		}
		void PlatformFile::Write(Handle Handle, float128 Data)
		{
			WriteFormatted(Handle, "f", Data);
		}
	}
}