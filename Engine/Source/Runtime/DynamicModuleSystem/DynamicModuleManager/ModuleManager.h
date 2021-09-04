// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <DynamicModuleDefinition\IDynamicModule.h>
#include <MemoryManagement\Singleton.h>
#include <Common\SpinLock.h>
#include <Containers\Strings.h>
#include <Containers\Promise.h>
#include <Containers\Delegate.h>
#include <Containers\Queue.h>
#include <Containers\Map.h>
#include <Platform\PlatformOS.h>
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Platform;
	using namespace DynamicModuleDefinition;
	using namespace Containers;
	using namespace Threading;

	namespace DynamicModuleManager
	{
		class ModuleManager
		{
		public:
			typedef Delegate<const String&> ModuleStatusEventHandler;

		private:
			struct ModuleInfo
			{
			public:
				PlatformOS::Handle Handle;
				IDynamicModule* DynamicModule;
			};

			typedef std::shared_ptr<std::function<void(void)>> Task;
			typedef Queue<Task> TaskQueue;
			typedef Map<String, ModuleInfo> LibraryMap;

			SINGLETON_DECLARATION(ModuleManager);

		private:
			ModuleManager(void);
			~ModuleManager(void);

			Promise<bool> Load(const String& Name);
			Promise<bool> Unload(const String& Name);

			bool IsLoaded(const String& Name);

		private:
			bool LoadInternal(const String& Name);

			void ThreadWorker(void);

		public:
			ModuleStatusEventHandler OnModuleLoading;
			ModuleStatusEventHandler OnModuleLoaded;
			ModuleStatusEventHandler OnModuleLoadingFailed;

		private:
			Thread m_Thread;
			TaskQueue m_TaskQueue;
			SpinLock m_Lock;
			LibraryMap m_Libraries;
		};
	}
}

#endif