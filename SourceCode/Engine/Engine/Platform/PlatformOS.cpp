// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformOS.h>
#include <Windows.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		PlatformOS::Handle PlatformOS::GetModuleInstance(cwstr ModuleName)
		{
			return (PlatformOS::Handle)GetModuleHandleW(ModuleName);
		}

		PlatformOS::Handle PlatformOS::GetExecutingModuleInstance(void)
		{
			return GetModuleInstance(nullptr);
		}

		void PlatformOS::GetExecutablePath(wstr Path)
		{
			GetModuleFileNameW(NULL, Path, _MAX_PATH);
		}

		int32 PlatformOS::GetErrorCode(void)
		{
			return GetLastError();
		}

		void PlatformOS::GetErrorMessage(str *Message)
		{
			int32 code = GetErrorCode();

			if (code == 0)
				return;

			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)Message, 0, NULL);
		}

		void PlatformOS::GenerateGUID(str *ID)
		{
			UUID uuid;
			UuidCreate(&uuid);
			UuidToStringA(&uuid, (RPC_CSTR*)ID);
		}
	}
}
#endif

//GetExecutingDirectory in other os or just Linux
//#include <unistd.h>
//#define GetCurrentDir getcwd