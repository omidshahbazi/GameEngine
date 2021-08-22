// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\Logger.h>
#include <Common\BitwiseUtils.h>
#include <Containers\StringUtility.h>
#include <Containers\Exception.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformOS.h>
#include <FileUtility\Path.h>
#include <Debugging\CoreDebug.h>
#include <stdarg.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace FileUtility;

	namespace Debugging
	{
#define INSERT_LOG() \
			va_list args; \
			va_start(args, Content); \
			InsertLog(Level, CategoryFlags, File.GetValue(), LineNumber, Function.GetValue(), Content.GetValue(), args); \
			va_end(args);

		Logger::Logger(const WString& FilePath) :
			m_QueueSize(0),
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

			INSERT_LOG();
		}

		void Logger::Put(Levels Level, const String Content, va_list Args)
		{
			String File;
			uint32 LineNumber = 0;
			String Function;
			Categories CategoryFlags = Categories::Default;

			InsertLog(Level, CategoryFlags, File.GetValue(), LineNumber, Function.GetValue(), Content.GetValue(), Args);
		}

		void Logger::Put(Levels Level, Categories CategoryFlags, const String Content, ...)
		{
			String File;
			uint32 LineNumber = 0;
			String Function;

			INSERT_LOG();
		}

		void Logger::Put(Levels Level, Categories CategoryFlags, const String Content, va_list Args)
		{
			String File;
			uint32 LineNumber = 0;
			String Function;

			InsertLog(Level, CategoryFlags, File.GetValue(), LineNumber, Function.GetValue(), Content.GetValue(), Args);
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...)
		{
			Categories CategoryFlags = Categories::Default;

			INSERT_LOG();
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, va_list Args)
		{
			Categories CategoryFlags = Categories::Default;

			InsertLog(Level, CategoryFlags, File.GetValue(), LineNumber, Function.GetValue(), Content.GetValue(), Args);
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...)
		{
			INSERT_LOG();
		}

		void Logger::Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, va_list Args)
		{
			InsertLog(Level, CategoryFlags, File.GetValue(), LineNumber, Function.GetValue(), Content.GetValue(), Args);
		}

		void Logger::Put(const Exception& Exception)
		{
			String content = Exception.GetWhat();
			if (Exception.GetInfo().GetLength() != 0)
				content += StringUtility::Format<char8>(" Info: %s", Exception.GetInfo().GetValue());

			InsertLog(Levels::Exception, Exception.GetCategoryFlags(), Exception.GetFile().GetValue(), Exception.GetLineNumber(), Exception.GetFunction().GetValue(), content.GetValue());
		}

		void Logger::InsertLog(Levels Level, Categories CategoryFlags, cstr File, uint32 LineNumber, cstr Function, cstr Content, va_list Args)
		{
			char8 content[2048];
			StringUtility::Format(content, Content, Args);

			InsertLog(Level, CategoryFlags, File, LineNumber, Function, content);
		}

		void Logger::InsertLog(Levels Level, Categories CategoryFlags, cstr File, uint32 LineNumber, cstr Function, cstr Content)
		{
#define SET_STRING(ValueRef, Parameter) \
			(ValueRef).Length = CharacterUtility::GetLength(Parameter); \
			if (Parameter != nullptr) \
				PlatformMemory::Copy(Parameter, (ValueRef).Value, (ValueRef).Length); \
			(ValueRef).Value[(ValueRef).Length] = '\0';

			if (Level < m_MinimumLevel)
				return;

			if ((int)(m_CategoryMask & CategoryFlags) == 0)
				return;

			Log log = {};
			log.Level = Level;
			log.CategoryFlags = CategoryFlags;
			SET_STRING(log.File, File);
			log.LineNumber = LineNumber;
			SET_STRING(log.Function, Function);
			SET_STRING(log.Content, Content);

			m_Lock.Lock();
			{
				m_LogQueue[m_QueueSize++] = log;
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
				if (m_QueueSize == 0)
				{
					PlatformThread::YieldThread();
					continue;
				}

				if (m_Lock.TryLock(1))
				{
					for (uint8 i = 0; i < m_QueueSize; ++i)
					{
						const Log& log = m_LogQueue[i];

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
								PlatformFile::Write(handle, log.Content.Value);

								PlatformFile::Write(handle, "\nIn ");

								PlatformFile::Write(handle, log.File.Value);
								PlatformFile::Write(handle, ":Ln");
								PlatformFile::Write(handle, log.LineNumber);

								PlatformFile::Write(handle, " ");
								PlatformFile::Write(handle, log.Function.Value);

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

					m_QueueSize = 0;

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