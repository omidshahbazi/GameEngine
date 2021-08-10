// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <LogSystem\Logger.h>
#include <Common\BitwiseUtils.h>
#include <Containers\StringUtility.h>
#include <Containers\Exception.h>
#include <Platform\PlatformFile.h>
#include <Utility\Path.h>
#include <stdarg.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace Utility;

	namespace LogSystem
	{
#define INSERT_LOG() \
			va_list args; \
			va_start(args, Content); \
			InsertLog(Level, CategoryFlags, File, LineNumber, Function, Content, args); \
			va_end(args);

		Logger::Logger(const WString& FilePath) :
			m_FilePath(FilePath),
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

		void Logger::Put(Levels Level, const String Content, ...)
		{
			String File;
			uint32 LineNumber = 0;
			String Function;
			Categories CategoryFlags = Categories::Default;

			INSERT_LOG(Content);
		}

		void Logger::Put(Levels Level, Categories CategoryFlags, const String Content, ...)
		{
			String File;
			uint32 LineNumber = 0;
			String Function;

			INSERT_LOG();
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...)
		{
			Categories CategoryFlags = Categories::Default;
			INSERT_LOG();
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...)
		{
			INSERT_LOG();
		}

		void Logger::Put(const Exception& Exception)
		{
			String content = Exception.GetMessage();
			if (Exception.GetInfo().GetLength() != 0)
				content += StringUtility::Format<char8>(" Info: %s", Exception.GetInfo().GetValue());

			InsertLog(Levels::Exception, Exception.GetCategoryFlags(), Exception.GetFile(), Exception.GetLineNumber(), Exception.GetFunction(), content);
		}

		void Logger::InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const String& Content, va_list Args)
		{
			InsertLog(Level, CategoryFlags, File, LineNumber, Function, StringUtility::Format(Content, Args));
		}

		void Logger::InsertLog(Levels Level, Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function, const String& Content)
		{
			if (Level < m_MinimumLevel)
				return;

			if ((int)(m_CategoryMask & CategoryFlags) == 0)
				return;

			Log log = {};
			log.Level = Level;
			log.CategoryFlags = CategoryFlags;
			log.File = File;
			log.LineNumber = LineNumber;
			log.Function = Function;
			log.Content = Content;

			m_Lock.Lock();
			{
				m_Logs.Enqueue(log);
			}
			m_Lock.Release();
		}

		void Logger::ThreadWorker(void)
		{
			PlatformFile::Handle handle = 0;

			if (m_FilePath.GetLength() != 0)
			{
				if (PlatformFile::Exists(m_FilePath.GetValue()))
					PlatformFile::Move(m_FilePath.GetValue(), Path::ChangeExtension(m_FilePath, L".Previous." + Path::GetExtension(m_FilePath)).GetValue());

				handle = PlatformFile::Open(m_FilePath.GetValue(), PlatformFile::OpenModes::Output);
			}

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
							if (handle != 0)
							{
								switch (log.Level)
								{
								case Levels::Info: PlatformFile::Write(handle, "[Info]"); break;
								case Levels::Warning: PlatformFile::Write(handle, "[Warning]"); break;
								case Levels::Error: PlatformFile::Write(handle, "[Error]"); break;
								case Levels::Exception: PlatformFile::Write(handle, "[Exception]"); break;
								case Levels::Fatal: PlatformFile::Write(handle, "[Fatal]"); break;
								}

								PlatformFile::Write(handle, "\n");
								PlatformFile::Write(handle, log.Content.GetValue());

								PlatformFile::Write(handle, "\nIn ");

								PlatformFile::Write(handle, log.File.GetValue());
								PlatformFile::Write(handle, ":Ln");
								PlatformFile::Write(handle, log.LineNumber);

								PlatformFile::Write(handle, " ");
								PlatformFile::Write(handle, log.Function.GetValue());

								PlatformFile::Write(handle, "\n");
							}

							CALL_CALLBACK(IListener, OnLog, log);
						}
						catch (Exception& ex)
						{
						}
						catch (...)
						{

						}
					}

					m_Lock.Release();
				}
			}

			if (handle != 0)
				PlatformFile::Close(handle);
		}

#undef INSERT_LOG_LEVEL_2
#undef INSERT_LOG_LEVEL_1
	}
}