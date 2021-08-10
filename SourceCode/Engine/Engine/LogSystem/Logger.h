// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <Common\SpinLock.h>
#include <Containers\Strings.h>
#include <Containers\ListenerContainer.h>
#include <Containers\Queue.h>
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

			enum class Categories
			{
				None = 0,
				Default = 1,
				CoreSystem = (Default << 1),
				EditorGUI = (CoreSystem << 1),
				FontSystem = (EditorGUI << 1),
				GameObjectSystem = (FontSystem << 1),
				InputSystem = (GameObjectSystem << 1),
				Parallelizing = (InputSystem << 1),
				Rendering = (Parallelizing << 1),
				ResourceSystem = (Rendering << 1),
				All = 0xFFFFFFF
			};

			struct Log
			{
			public:
				Levels Level;
				Categories CategoryFlags;
				String File;
				uint32 LineNumber;
				String Function;
				WString Content;
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

			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const WString Content, ...);

			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const WString Content, ...);

		private:
			void InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const WString& Content, va_list Args);

			void ThreadWorker(void);

		private:
			Thread m_WorkerThread;

			Queue<Log> m_Logs;
			SpinLock m_Lock;

			Levels m_MinimumLevel;
			Categories m_CategoryMask;
		};

#define LogInfoooo(LoggerPtr, Level, Content, ...) (LoggerPtr)->Put(DEBUG_ARGUMENTS, Level, Content, __VA_ARGS__)
	}
}

#endif