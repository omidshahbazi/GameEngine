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
		//https://stackoverflow.com/questions/384121/creating-a-module-system-dynamic-loading-in-c
		PlatformOS::Handle PlatformOS::LoadModule(cwstr ModuleFileName)
		{
			return (PlatformOS::Handle)LoadLibraryW(ModuleFileName);
		}

		void PlatformOS::UnloadModule(Handle Handle)
		{
			FreeLibrary((HMODULE)Handle);
		}

		PlatformOS::Handle PlatformOS::GetModuleInstance(cwstr ModuleName)
		{
			return (PlatformOS::Handle)GetModuleHandleW(ModuleName);
		}

		void* PlatformOS::GetProcedureAddress(Handle Handle, cstr Name)
		{
			return GetProcAddress((HMODULE)Handle, Name);
		}

		PlatformOS::Handle PlatformOS::GetExecutingModuleInstance(void)
		{
			return GetModuleInstance(nullptr);
		}

		void PlatformOS::GetExecutablePath(wstr Path)
		{
			GetModuleFileNameW(nullptr, Path, _MAX_PATH);
		}

		void PlatformOS::GetExecutableExtension(str Extension)
		{
			PlatformMemory::Copy(".exe", Extension, 4);
		}

		void PlatformOS::GetDynamicLinkLibraryExtension(str Extension)
		{
			PlatformMemory::Copy(".dll", Extension, 4);
		}

		int32 PlatformOS::GetErrorCode(void)
		{
			return GetLastError();
		}

		void PlatformOS::GetErrorMessage(str* Message)
		{
			int32 code = GetErrorCode();

			if (code == 0)
				return;

			FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)Message, 0, nullptr);
		}

		void PlatformOS::GenerateGUID(GUIDData* Data)
		{
			UUID uuid;
			UuidCreate(&uuid);

			Data->Part1[0] = (uuid.Data1 >> 24) & 0xFF;
			Data->Part1[1] = (uuid.Data1 >> 16) & 0xFF;
			Data->Part1[2] = (uuid.Data1 >> 8) & 0xFF;
			Data->Part1[3] = uuid.Data1 & 0xFF;

			Data->Part2[0] = (uuid.Data2 >> 8) & 0xFF;
			Data->Part2[1] = uuid.Data2 & 0xFF;

			Data->Part3[0] = (uuid.Data3 >> 8) & 0xFF;
			Data->Part3[1] = uuid.Data3 & 0xFF;

			Data->Part4[0] = uuid.Data4[0];
			Data->Part4[1] = uuid.Data4[1];

			Data->Part5[0] = uuid.Data4[2];
			Data->Part5[1] = uuid.Data4[3];
			Data->Part5[2] = uuid.Data4[4];
			Data->Part5[3] = uuid.Data4[5];
			Data->Part5[4] = uuid.Data4[6];
			Data->Part5[5] = uuid.Data4[7];
		}

		void PlatformOS::GetRoamingPath(wstr Path)
		{
			auto path = std::filesystem::temp_directory_path().parent_path().parent_path().parent_path();

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