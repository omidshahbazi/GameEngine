// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FileUtility\FileWatcher.h>
#include <Platform\PlatformFile.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace FileUtility
	{
		FileWatcher::FileWatcher(const WString& FullPath) :
			m_FullPath(FullPath)
		{
			m_Thread.Initialize([&](void*) { ThreadWorker(); });
			m_Thread.SetName("File Watcher Thread");
		}

		FileWatcher::~FileWatcher(void)
		{
			m_Thread.Shutdown(true).Wait();
		}

		void FileWatcher::ThreadWorker(void) const
		{
			const uint16 WATCHER_INFO_COUNT = 512;

			PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(m_FullPath.GetValue(), false);

			PlatformFile::WatchInfo watchInfos[WATCHER_INFO_COUNT];

			while (!m_Thread.GetShouldExit())
			{
				uint32 len;
				PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, WATCHER_INFO_COUNT, len);

				if (len == 0)
				{
					PlatformThread::Sleep(1000);

					continue;
				}

				for (uint32 i = 0; i < len; ++i)
				{
					PlatformFile::WatchInfo& info = watchInfos[i];

					WString file(info.FileName, info.FileNameLength);

					switch (info.Action)
					{
					case PlatformFile::WatchAction::Added: OnFileAddedEvent(file); break;
					case PlatformFile::WatchAction::Removed: OnFileRemovedEvent(file); break;
					case PlatformFile::WatchAction::Modified: OnFileModifiedEvent(file); break;
					case PlatformFile::WatchAction::RenamedOldName: OnFileRenamedFromEvent(file); break;
					case PlatformFile::WatchAction::RenamedNewName: OnFileRenamedToEvent(file); break;
					}
				}

				PlatformThread::YieldThread();
			}
		}
	}
}