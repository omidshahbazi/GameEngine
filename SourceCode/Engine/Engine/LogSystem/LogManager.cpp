// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <LogSystem\LogManager.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Platform;
	using namespace FileUtility;

	namespace LogSystem
	{
		SINGLETON_DEFINITION(LogManager);

		LogManager::LogManager(void) :
			m_Logger(nullptr)
		{
			char16 roamingPath[128];
			PlatformOS::GetRoamingPath(roamingPath);

			WString path = Path::Combine(Path::Combine(WString(roamingPath), String(EngineName).ChangeType<char16>()), WString(L"Logs"));
			if (!PlatformDirectory::Exists(path.GetValue()))
				PlatformDirectory::Create(path.GetValue());

			m_Logger = new Logger(Path::Combine(path, WString(L"Core.log")));
		}

		LogManager::~LogManager(void)
		{
			delete m_Logger;
		}
	}
}