// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <LogSystem\Logger.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace LogSystem
	{
		class LOGSYSTEM_API LogManager
		{
			SINGLETON_DECLARATION(LogManager)

		private:
			LogManager(void);
			~LogManager(void);

		public:
			Logger* GetCoreLogger(void) const
			{
				return m_Logger;
			}

		private:
			Logger* m_Logger;
		};
	}
}

#endif