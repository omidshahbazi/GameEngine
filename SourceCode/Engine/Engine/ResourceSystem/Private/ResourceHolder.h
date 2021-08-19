// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceCompiler.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\Utilities.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Containers;
	using namespace FileUtility;

	namespace Rendering
	{
		class RenderWindow;
		class RenderContext;
	}

	namespace ResourceSystem
	{
		namespace Private
		{
			class RESOURCESYSTEM_API ResourceHolder : ResourceCompiler::IListener
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
					String ID;
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

					AddLoadTask(FilePath, type, resource);

					AddToLoaded(FilePath, type, resource);

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

				template<typename T>
				Resource<T>* LoadFromMemory(const String& Name, const ByteBuffer& Buffer)
				{
					return LoadFromMemory<T>(Name.ChangeType<char16>(), Buffer);
				}

				template<typename T>
				Resource<T>* LoadFromMemory(const WString& Name, const ByteBuffer& Buffer)
				{
					ResourceTypes type = ResourceTypeSpecifier<T>::Type;

					Resource<T>* resource = AllocateResource(resource);

					LoadInternal(Buffer, type, resource);

					AddToLoaded(Name, type, resource);

					return resource;
				}

				void Unload(ResourceBase* Resource);

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
				void AddLoadTask(const WString& FilePath, ResourceTypes Type, ResourceBase* ResourcePtr);

#if DEBUG_MODE
				void LoadInternal(const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr, const WString& Name);
#else
				void LoadInternal(const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr);
#endif

				ResourceBase* GetFromLoaded(const WString& Name);
				void AddToLoaded(const WString& Name, ResourceTypes Type, ResourceBase* Resource);

				template<typename T>
				Resource<T>* AllocateResource(T* ResourcePtr) const
				{
					Resource<T>* handle = ResourceSystemAllocators::ResourceAllocator_Allocate<Resource<T>>();
					Construct(handle, ResourcePtr);

					return handle;
				}

				ResourceTypes GetResourceType(ResourceBase* Resource);

				void IOThreadWorker(void);

				void OnResourceCompiled(const WString& FullPath, uint32 Hash, const String& ResourceID) override;

			private:
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