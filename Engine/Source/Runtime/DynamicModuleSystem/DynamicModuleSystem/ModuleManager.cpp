// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleSystem\ModuleManager.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace FileUtility;

	namespace DynamicModuleSystem
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

		Promise<void*> ModuleManager::Load(const String& Name)
		{
			if (Name == String::Empty)
				return nullptr;

			PromiseBlock<void*>* promise = AllocatePromiseBlock<void*>(RootAllocator::GetInstance(), 1);

			if (m_Libraries.Contains(Name))
			{
				ModuleInfo& info = m_Libraries[Name];

				promise->SetValue(info.Pointer);
				promise->IncreaseDoneCount();
			}
			else
			{
				m_Lock.Lock();
				m_TaskQueue.Enqueue(std::make_shared<Task::weak_type::element_type>([&]()
					{
						promise->SetValue(LoadInternal(Name));
						promise->IncreaseDoneCount();
					}));
				m_Lock.Release();
			}

			return promise;
		}

		Promise<bool> ModuleManager::Unload(const String& Name)
		{
			ModuleInfo* info = nullptr;

			if (m_Libraries.Contains(Name))
				info = &m_Libraries[Name];

			return UnloadInternal(Name, info);
		}

		Promise<bool> ModuleManager::Unload(void* Pointer)
		{
			String name;
			ModuleInfo* info = nullptr;

			for (auto& item : m_Libraries)
				if (item.GetSecond().Pointer == Pointer)
				{
					name = item.GetFirst();
					info = &item.GetSecond();
					break;
				}

			return UnloadInternal(name, info);
		}

		void* ModuleManager::LoadInternal(const String& Name)
		{
			char8 extension[5];
			extension[4] = '\0';
			PlatformOS::GetDynamicLinkLibraryExtension(extension);

			WString path = Path::Combine(FileSystem::GetExecutableDirectory(), Name.ChangeType<char16>()) + String(extension).ChangeType<char16>();

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

					if (module != nullptr)
					{
						void* pointer = module->Load();
						if (pointer != nullptr)
						{
							OnModuleLoaded(Name, pointer);

							m_Libraries[Name] = { moduleHandle, module , pointer };

							return pointer;
						}
					}

					PlatformOS::UnloadModule(moduleHandle);
				}
			}

			OnModuleLoadingFailed(Name);

			return nullptr;
		}

		PromiseBlock<bool>* ModuleManager::UnloadInternal(const String& Name, ModuleInfo* Info)
		{
			PromiseBlock<bool>* promise = AllocatePromiseBlock<bool>(RootAllocator::GetInstance(), 1);

			if (Info == nullptr)
			{
				promise->SetValue(false);
				promise->IncreaseDoneCount();
			}
			else
			{
				ModuleInfo info = *Info;

				m_Lock.Lock();
				m_TaskQueue.Enqueue(std::make_shared<Task::weak_type::element_type>([&]()
					{
						if (!info.DynamicModule->Unload())
							promise->SetValue(false);
						else
						{
							Destruct(info.DynamicModule);
							DeallocateMemory(RootAllocator::GetInstance(), info.DynamicModule);

							PlatformOS::UnloadModule(info.Handle);

							promise->SetValue(true);
						}

						m_Libraries.Remove(Name);

						promise->IncreaseDoneCount();
					}));
				m_Lock.Release();
			}

			return promise;
		}

		void ModuleManager::ThreadWorker(void)
		{
			while (!m_Thread.GetShouldExit())
			{
				PlatformThread::Sleep(1000);

				if (!m_Lock.TryLock())
					continue;

				if (m_TaskQueue.GetSize() == 0)
				{
					m_Lock.Release();
					continue;
				}

				Task task;
				m_TaskQueue.Dequeue(&task);
				m_Lock.Release();

				(*task)();

				PlatformThread::YieldThread();
			}
		}
	}
}