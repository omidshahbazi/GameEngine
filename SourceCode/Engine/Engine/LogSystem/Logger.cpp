// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <LogSystem\Logger.h>
#include <Common\BitwiseUtils.h>
#include <stdarg.h>

namespace Engine
{
	namespace LogSystem
	{
#define INSERT_LOG_LEVEL_1(Format) \
			Categories CategoryFlags = Categories::Default; \
			INSERT_LOG_LEVEL_2(Format)

#define INSERT_LOG_LEVEL_2(Format) \
			va_list args; \
			va_start(args, Content); \
			InsertLog(Level, CategoryFlags, File, LineNumber, Function, Format, args); \
			va_end(args);

		Logger::Logger(const WString& FilePath) :
			m_MinimumLevel(Levels::Info),
			m_CategoryMask(Categories::All)
		{
			m_WorkerThread.Initialize([this](void*) { ThreadWorker(); });
			m_WorkerThread.SetName("LoggerThread");
		}

		Logger::~Logger(void)
		{
			m_WorkerThread.Shutdown().Wait();
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...)
		{
			INSERT_LOG_LEVEL_1(Content.ChangeType<char16>());
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const WString Content, ...)
		{
			INSERT_LOG_LEVEL_1(Content);
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...)
		{
			INSERT_LOG_LEVEL_2(Content.ChangeType<char16>());
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const WString Content, ...)
		{
			INSERT_LOG_LEVEL_2(Content);
		}

		void Logger::InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const WString& Content, va_list Args)
		{
			if (Level < m_MinimumLevel)
				return;

			if ((int)(m_CategoryMask & CategoryFlags) == 0)
				return;

			char16 content[1024] = {};
			uint16 size = vswprintf(content, Content.GetValue(), Args);

			Log log = {};
			log.Level = Level;
			log.CategoryFlags = CategoryFlags;
			log.File = File;
			log.LineNumber = LineNumber;
			log.Function = Function;
			log.Content = WString(content, size);

			m_Lock.Lock();
			{
				m_Logs.Enqueue(log);
			}
			m_Lock.Release();
		}

		void Logger::ThreadWorker(void)
		{
			while (!m_WorkerThread.GetShouldExit())
			{
				if (m_Logs.GetSize() == 0)
				{
					PlatformThread::YieldThread();
					continue;
				}

				if (m_Lock.TryLock(1))
				{
					while (m_Logs.GetSize() != 0)
					{
						Log log;
						m_Logs.Dequeue(&log);

						try
						{
							//write in file

							CALL_CALLBACK(IListener, OnLog, log);
						}
						catch (std::exception ex)
						{

						}
					}

					m_Lock.Release();
				}
			}
		}

#undef INSERT_LOG_LEVEL_2
#undef INSERT_LOG_LEVEL_1
	}
}