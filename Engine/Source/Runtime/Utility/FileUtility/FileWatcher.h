// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FILE_WATCHER
#define FILE_WATCHER

#include <Threading\Thread.h>
#include <Containers\Delegate.h>

namespace Engine
{
	using namespace Containers;
	using namespace Threading;

	namespace FileUtility
	{
		class FILEUTILITY_API FileWatcher
		{
		public:
			typedef Delegate<const WString&> FileChangedEventHandler;

		public:
			FileWatcher(const WString& FullPath);
			~FileWatcher(void);

		private:
			void ThreadWorker(void) const;

		public:
			FileChangedEventHandler OnFileAddedEvent;
			FileChangedEventHandler OnFileRemovedEvent;
			FileChangedEventHandler OnFileModifiedEvent;
			FileChangedEventHandler OnFileRenamedFromEvent;
			FileChangedEventHandler OnFileRenamedToEvent;

		private:
			WString m_FullPath;
			Thread m_Thread;
		};
	}
}

#endif