// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformOS.h>
#include <Platform\PlatformMemory.h>
#include <Windows.h>
#include <filesystem>

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
			GetModuleFileNameW(nullptr, Path, _MAX_PATH);
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

			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)Message, 0, nullptr);
		}

		void PlatformOS::GenerateGUID(str *ID)
		{
			UUID uuid;
			UuidCreate(&uuid);
			UuidToStringA(&uuid, (RPC_CSTR*)ID);
		}

		void PlatformOS::GetRoamingPath(wstr Path)
		{
			auto path = std::filesystem::temp_directory_path().parent_path().parent_path();

			path /= "Roaming";

			if (!std::filesystem::exists(path))
				std::filesystem::create_directories(path);

			auto wPath = path.wstring();
			PlatformMemory::Copy(wPath.c_str(), Path, wPath.length());
		}

		bool PlatformOS::IsDebuggerAttached(void)
		{
			return IsDebuggerPresent();
		}
	}
}
#endif

//GetExecutingDirectory in other os or just Linux
//#include <unistd.h>
//#define GetCurrentDir getcwd