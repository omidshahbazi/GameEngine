// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Debugging\Logger.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace Debugging
	{
		class DEBUGGING_API LogManager
		{
			CREATOR_DECLARATION(LogManager);
			DESTROYER_DECLARATION(LogManager);
			GET_INSTANCE_DECLARATION(LogManager);

		private:
			LogManager(void);
			~LogManager(void);

		public:
			Logger* GetCoreLogger(void) const
			{
				return m_CoreLogger;
			}

			Logger* GetPlayerLogger(void) const
			{
				return m_PlayerLogger;
			}

		private:
			Logger* m_CoreLogger;
			Logger* m_PlayerLogger;
		};
	}
}

#endif