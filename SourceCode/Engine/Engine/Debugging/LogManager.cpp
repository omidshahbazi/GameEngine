// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\LogManager.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Platform;
	using namespace FileUtility;

	namespace Debugging
	{
		CREATOR_DEFINITION(LogManager);

		LogManager::LogManager(void) :
			m_CoreLogger(nullptr),
			m_PlayerLogger(nullptr)
		{
			char16 roamingPath[128];
			PlatformOS::GetRoamingPath(roamingPath);

			WString path = Path::Combine(Path::Combine(WString(roamingPath), String(EngineName).ChangeType<char16>()), WString(L"Logs"));
			if (!PlatformDirectory::Exists(path.GetValue()))
				PlatformDirectory::Create(path.GetValue());

			m_CoreLogger = new Logger(Path::Combine(path, WString(L"Core.log")));

			m_CoreLogger->Put(Logger::Levels::Error, "The m_PlayerLogger is still on the m_CoreLogger");
			m_PlayerLogger = m_CoreLogger;
		}

		LogManager::~LogManager(void)
		{
			delete m_CoreLogger;
			//delete m_PlayerLogger;
		}
	}
}