// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <Common\SpinLock.h>
#include <Common\Categories.h>
#include <Containers\Strings.h>
#include <Containers\ListenerContainer.h>
#include <Containers\Queue.h>
#include <Containers\Exception.h>
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Containers;
	using namespace Threading;

	namespace LogSystem
	{
		class LOGSYSTEM_API Logger
		{
		public:
			enum class Levels
			{
				Info,
				Warning,
				Error,
				Exception,
				Fatal
			};

			struct Log
			{
			public:
				Levels Level;
				Categories CategoryFlags;
				String File;
				uint32 LineNumber;
				String Function;
				String Content;
			};

			class LOGSYSTEM_API IListener
			{
			public:
				virtual void OnLog(const Log& Log)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			Logger(const WString& FilePath);
			~Logger(void);

			void Put(Levels Level, const String Content, ...);
			void Put(Levels Level, Categories CategoryFlags, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...);
			void Put(const Exception& Exception);

			Levels GetMinimumLevel(void) const
			{
				return m_MinimumLevel;
			}
			void SetMinimumLevel(Levels Level)
			{
				m_MinimumLevel = Level;
			}

			Categories GetCategoryMask(void) const
			{
				return m_CategoryMask;
			}
			void SetCategoryMask(Categories CategoryMask)
			{
				m_CategoryMask = CategoryMask;
			}

		private:
			void InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const String& Content, va_list Args);
			void InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const String& Content);

			void ThreadWorker(void);

		private:
			Thread m_WorkerThread;

			Queue<Log> m_Logs;
			SpinLock m_Lock;

			WString m_FilePath;
			Levels m_MinimumLevel;
			Categories m_CategoryMask;
		};

#define LogInfoooo(LoggerPtr, Level, Content, ...) (LoggerPtr)->Put(DEBUG_ARGUMENTS, Level, Content, __VA_ARGS__)
	}
}

#endif