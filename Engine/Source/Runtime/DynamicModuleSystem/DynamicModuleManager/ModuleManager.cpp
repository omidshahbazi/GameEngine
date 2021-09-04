// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleManager\ModuleManager.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace FileUtility;

	namespace DynamicModuleManager
	{
		SINGLETON_DEFINITION(ModuleManager);

		ModuleManager::ModuleManager(void)
		{
			m_Thread.Initialize([&](void*) { ThreadWorker(); });
			m_Thread.SetName("Module Manager Thread");
		}

		ModuleManager::~ModuleManager(void)
		{
			m_Thread.Shutdown(true).Wait();
		}

		Promise<bool> ModuleManager::Load(const String& Name)
		{
			PromiseBlock<bool>* promise = AllocatePromiseBlock<bool>(RootAllocator::GetInstance(), 1);

			m_Lock.Lock();
			m_TaskQueue.Enqueue(std::make_shared<Task::weak_type::element_type>([&]()
				{
					promise->SetValue(LoadInternal(Name));
					promise->IncreaseDoneCount();
				}));
			m_Lock.Release();

			return promise;
		}

		Promise<bool> ModuleManager::Unload(const String& Name)
		{
			PromiseBlock<bool>* promise = AllocatePromiseBlock<bool>(RootAllocator::GetInstance(), 1);

			if (m_Libraries.Contains(Name))
			{
				promise->SetValue(false);
				promise->IncreaseDoneCount();
			}
			else
			{
				m_Lock.Lock();
				m_TaskQueue.Enqueue(std::make_shared<Task::weak_type::element_type>([&]()
					{
						const ModuleInfo& info = m_Libraries[Name];

						if (!info.DynamicModule->Unload())
							promise->SetValue(false);
						else
						{
							PlatformOS::UnloadModule(info.Handle);

							promise->SetValue(true);
						}

						promise->IncreaseDoneCount();

						m_Libraries.Remove(Name);
					}));
				m_Lock.Release();
			}

			return promise;
		}

		bool ModuleManager::IsLoaded(const String& Name)
		{
			return m_Libraries.Contains(Name);
		}

		bool ModuleManager::LoadInternal(const String& Name)
		{
			str extension;
			PlatformOS::GetDynamicLinkLibraryExtension(&extension);

			WString path = Path::Combine(FileSystem::GetExecutableDirectory(), Name.ChangeType<char16>(), String(extension).ChangeType<char16>());

			cstr CREATE_PROCEDURE_NAME = "Create";
			typedef IDynamicModule* (*CreateProcedure)(AllocatorBase*);

			OnModuleLoading(Name);

			PlatformOS::Handle moduleHandle = PlatformOS::LoadModule(path.GetValue());
			if (moduleHandle != 0)
			{
				CreateProcedure createFunction = ReinterpretCast(CreateProcedure, PlatformOS::GetProcedureAddress(moduleHandle, CREATE_PROCEDURE_NAME));
				if (createFunction != nullptr)
				{
					IDynamicModule* module = createFunction(RootAllocator::GetInstance());

					if (module->Load())
					{
						OnModuleLoaded(Name);

						m_Libraries[Name] = { moduleHandle, module };

						return true;
					}
					else
					{
						PlatformOS::UnloadModule(moduleHandle);
					}
				}
			}

			OnModuleLoadingFailed(Name);

			return false;
		}

		void ModuleManager::ThreadWorker(void)
		{
			while (!m_Thread.GetShouldExit())
			{
				if (m_TaskQueue.GetSize() == 0)
					PlatformThread::Sleep(1000);

				Task task;
				m_TaskQueue.Dequeue(&task);

				(*task)();

				PlatformThread::YieldThread();
			}
		}
	}
}