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

	namespace DynamicModuleSystem
	{
		class DYNAMICMODULESYSTEM_API ModuleManager
		{
		public:
			typedef Delegate<const String&> ModuleStatusEventHandler;
			typedef Delegate<const String&, void*> ModuleLoadedEventHandler;

		private:
			struct ModuleInfo
			{
			public:
				PlatformOS::Handle Handle;
				IDynamicModule* DynamicModule;
				void* Pointer;
			};

			typedef std::shared_ptr<std::function<void(void)>> Task;
			typedef Queue<Task> TaskQueue;
			typedef Map<String, ModuleInfo> LibraryMap;

			SINGLETON_DECLARATION(ModuleManager);

		private:
			ModuleManager(void);
			~ModuleManager(void);

		public:
			Promise<void*> Load(const String& Name);

			template<typename T>
			T* Load(const String& Name)
			{
				auto module = Load(Name).Wait();
				if (module == nullptr)
					return nullptr;

				return ReinterpretCast(T*, module);
			}

			Promise<bool> Unload(const String& Name);

		private:
			void* LoadInternal(const String& Name);

			void ThreadWorker(void);

		public:
			ModuleStatusEventHandler OnModuleLoading;
			ModuleStatusEventHandler OnModuleLoadingFailed;
			ModuleLoadedEventHandler OnModuleLoaded;

		private:
			Thread m_Thread;
			TaskQueue m_TaskQueue;
			SpinLock m_Lock;
			LibraryMap m_Libraries;
		};
	}
}

#endif