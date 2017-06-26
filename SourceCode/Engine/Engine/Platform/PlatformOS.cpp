// Copyright 2016-2017 ?????????????. All Rights Reserved.
#ifdef WINDOWS
#include <Platform\PlatformOS.h>
#include <Windows.h>

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

		int32 PlatformOS::GetErrorCode(void)
		{
			return GetLastError();
		}

		void PlatformOS::GetErrorMessage(cstr *Message)
		{
			int32 code = GetErrorCode();

			if (code == 0)
				return;

			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)Message, 0, NULL);
		}
	}
}
#endif