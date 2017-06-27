// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformOS.h>
#include <Windows.h>
#include <direct.h>
#include <stdio.h>
#include <experimental\filesystem>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformOS::Handle PlatformOS::GetModuleInstance(cstr ModuleName)
		{
			return (PlatformOS::Handle)GetModuleHandle(ModuleName);
		}

		PlatformOS::Handle PlatformOS::GetExecutingModuleInstance(void)
		{
			return GetModuleInstance(nullptr);
		}

		void PlatformOS::GetExecutingDirectory(str Directory)
		{
			//_getcwd(Directory, FILENAME_MAX);

			//GetModuleFileName((HMODULE)GetExecutingModuleInstance(), Directory, FILENAME_MAX);

			std::experimental::filesystem::path path = std::experimental::filesystem::current_path();
			std::string str = path.string();

			str += "/";

			if (IsDebuggerPresent())
				str += "../Binaries/";

			memcpy(Directory, str.c_str(), str.size());
		}

		int32 PlatformOS::GetErrorCode(void)
		{
			return GetLastError();
		}

		cstr PlatformOS::GetErrorMessage(void)
		{
			int32 code = GetErrorCode();

			if (code == 0)
				return nullptr;

			char8 text[256];
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)text, 0, NULL);

			return text;
		}
	}
}
#endif

//GetExecutingDirectory in other os or just Linux
//#include <unistd.h>
//#define GetCurrentDir getcwd