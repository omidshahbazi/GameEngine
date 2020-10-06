// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ResourceCompiler.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\Utilities.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Utility\Path.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering::Private::ShaderCompiler;

	namespace Rendering
	{
		class RenderWindow;
		class RenderContext;
	}

	namespace ResourceSystem
	{
		namespace Private
		{
			//TODO: Load assets async
			class RESOURCESYSTEM_API ResourceHolder : Compiler::IListener, ResourceCompiler::IListener
			{
			private:
				struct ResourceLoaderTask
				{
				public:
					ResourceLoaderTask(ResourceHolder* Holder, const WString& FilePath, ResourceTypes Type, ResourceBase* Resource) :
						Holder(Holder),
						FilePath(FilePath),
						Type(Type),
						Resource(Resource)
					{
					}

					void operator()(void);

				public:
					ResourceHolder* Holder;
					WString FilePath;
					ResourceTypes Type;
					ResourceBase* Resource;
				};

				typedef Queue<ResourceLoaderTask*> ResourceLoaderTaskQueue;

				struct ResourceInfo
				{
				public:
					ResourceTypes Type;
					ResourceBase* Resource;
				};

				typedef Map<uint32, ResourceInfo> ResourceMap;

			public:
				ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath);
				virtual ~ResourceHolder(void);

				template<typename T>
				Resource<T>* Load(const String& FilePath)
				{
					return Load<T>(FilePath.ChangeType<char16>());
				}

				template<typename T>
				Resource<T>* Load(const WString& FilePath)
				{
					ResourceBase* loadedResource = GetFromLoaded(FilePath);

					if (loadedResource != nullptr)
						return ReinterpretCast(Resource<T>*, loadedResource);

					Resource<T>* resource = AllocateResource<T>(nullptr);

					ResourceTypes type = ResourceTypeSpecifier<T>::Type;

					AddToLoaded(FilePath, type, resource);

					ResourceLoaderTask* task = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceLoaderTask>();
					Construct(task, this, FilePath, type, resource);

					m_ResourceLoaderTasksLock.Lock();
					m_ResourceLoaderTasks.Enqueue(task);
					m_ResourceLoaderTasksLock.Release();

					return resource;
				}

				template<typename T>
				Resource<T>* AddFromMemory(const String& Name, T* ResourcePtr)
				{
					return AddFromMemory<T>(Name.ChangeType<char16>(), ResourcePtr);
				}

				template<typename T>
				Resource<T>* AddFromMemory(const WString& Name, T* ResourcePtr)
				{
					Resource<T>* resource = AllocateResource(ResourcePtr);

					AddToLoaded(Name, ResourceTypeSpecifier<T>::Type, resource);

					return resource;
				}

				//TODO: fill this
				//template<typename T>
				//Resource<T>* LoadFromMemory(const String& Name, ??????????)
				//{
				//	return LoadFromMemory<T>(Name.ChangeType<char16>(), Resource);
				//}

				//template<typename T>
				//Resource<T>* LoadFromMemory(const WString& Name, ? ? ? ? ? ? ? ? ? ? )
				//{
				//	ResourceHandle<T>* handle = AllocateResource(Resource);

				//	AddToLoaded(Name, ResourceTypeSpecifier<T>::Type, handle);

				//	return handle;
				//}

				template<typename T>
				void Unload(Resource<T>* Resource)
				{
					UnloadInternal(ResourceTypeSpecifier<T>::Type, Resource);
				}

				virtual ResourceCompiler* GetCompiler(void)
				{
					return &m_Compiler;
				}

				virtual const WString& GetLibraryPath(void) const
				{
					return m_LibraryPath;
				}

			protected:
				template<typename T>
				Resource<T>* GetLoaded(const String& Name)
				{
					return GetLoaded<T>(Name.ChangeType<char16>());
				}

				template<typename T>
				Resource<T>* GetLoaded(const WString& Name)
				{
					return ReinterpretCast(Resource<T>*, GetFromLoaded(Name));
				}

			private:
				template<typename T>
				T* LoadInternal(const WString& FilePath)
				{
					WString finalPath = Utilities::GetDataFileName(FilePath);

					ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

					if (!Utilities::ReadDataFile(inBuffer, Path::Combine(GetLibraryPath(), finalPath)))
						return nullptr;

					return ResourceFactory::Create<T>(inBuffer);
				}

				void UnloadInternal(ResourceTypes Type, ResourceBase* Resource);

				ResourceBase* GetFromLoaded(const WString& Name);
				void AddToLoaded(const WString& Name, ResourceTypes Type, ResourceBase* Resource);

				template<typename T>
				Resource<T>* AllocateResource(T* ResourcePtr) const
				{
					Resource<T>* handle = ResourceSystemAllocators::ResourceAllocator_Allocate<Resource<T>>();
					Construct(handle, ResourcePtr);
					return handle;
				}

				void DeallocateResource(ResourceBase* Resource) const
				{
					ResourceSystemAllocators::ResourceAllocator_Deallocate(Resource);
				}

				void IOThreadWorker(void);

				bool FetchShaderSource(const String& Name, String& Source) override;

				void OnResourceCompiled(const WString& FullPath, uint32 Hash, const String& ResourceID) override;

			private:
				RenderWindow* m_ContextWindow;
				RenderContext* m_Context;
				ResourceCompiler m_Compiler;
				Thread m_IOThread;
				ResourceLoaderTaskQueue m_ResourceLoaderTasks;
				SpinLock m_ResourceLoaderTasksLock;
				WString m_LibraryPath;
				ResourceMap m_LoadedResources;
			};
		}
	}
}
#endif