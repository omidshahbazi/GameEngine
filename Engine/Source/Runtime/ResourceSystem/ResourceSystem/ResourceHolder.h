// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <ResourceCommon\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\ResourceCompiler.h>
#include <ResourceSystem\Private\Utilities.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Containers;
	using namespace FileUtility;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceDatabase;
		}

		using namespace Private;

		class RESOURCESYSTEM_API ResourceHolder
		{
		private:
			struct ResourceLoaderTask
			{
			public:
				ResourceLoaderTask(ResourceHolder* Holder, GUID GUID, ResourceTypes Type, ResourceBase* Resource) :
					Holder(Holder),
					GUID(GUID),
					Type(Type),
					Resource(Resource)
				{
				}

				void operator()(void);

			public:
				ResourceHolder* Holder;
				GUID GUID;
				ResourceTypes Type;
				ResourceBase* Resource;
			};

			typedef Queue<ResourceLoaderTask*> ResourceLoaderTaskQueue;

			struct ResourceInfo
			{
			public:
				GUID ID;
				ResourceTypes Type;
				ResourceBase* Resource;
			};

			typedef Map<uint32, ResourceInfo> ResourceMap;
			typedef Map<WString, ResourceInfo> ResourceByNameMap;

		public:
			ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath);
			virtual ~ResourceHolder(void);

			template<typename T>
			Resource<T>* Load(const GUID& GUID)
			{
				ResourceBase* loadedResource = GetFromLoaded(GUID);

				if (loadedResource != nullptr)
					return ReinterpretCast(Resource<T>*, loadedResource);

				bool doesResourceExists = DoesResourceExists(GUID);

				Resource<T>* resource = AllocateResource<T>(nullptr);

				ResourceTypes type = ResourceTypeSpecifier<T>::Type;

				if (doesResourceExists)
					resource->Set(nullptr);
				else
					AddLoadTask(GUID, type, resource);

				AddToLoaded(GUID, type, resource);

				return resource;
			}

			template<typename T>
			Resource<T>* Load(const String& FilePath)
			{
				return Load<T>(FilePath.ChangeType<char16>());
			}

			template<typename T>
			Resource<T>* Load(const WString& FilePath)
			{
				Resource<T>* resource = AllocateResource<T>(nullptr);

				GUID guid = FindGUID(FilePath);

				ResourceTypes type = ResourceTypeSpecifier<T>::Type;

				m_WaitingToCompile[FilePath] = { guid, type, resource };

				if (guid == GUID::Invalid)
					resource->Set(nullptr);

				return resource;
			}

			template<typename T>
			Resource<T>* AddFromMemory(const GUID& GUID, T* ResourcePtr)
			{
				Resource<T>* resource = AllocateResource(ResourcePtr);

				AddToLoaded(GUID, ResourceTypeSpecifier<T>::Type, resource);

				return resource;
			}

			template<typename T>
			Resource<T>* LoadFromMemory(const GUID& GUID, const ByteBuffer& Buffer)
			{
				ResourceTypes type = ResourceTypeSpecifier<T>::Type;

				Resource<T>* resource = AllocateResource(resource);

				LoadInternal(GUID, Buffer, type, resource);

				AddToLoaded(GUID, type, resource);

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
			Resource<T>* GetLoaded(const GUID& GUID) const
			{
				return ReinterpretCast(Resource<T>*, GetFromLoaded(GUID));
			}

		private:
			void AddLoadTask(const GUID& GUID, ResourceTypes Type, ResourceBase* ResourcePtr);
			void LoadInternal(const GUID& GUID, const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr);

			bool DoesResourceExists(const GUID& GUID) const;
			GUID FindGUID(const WString& RelativeFilePath) const;

			ResourceBase* GetFromLoaded(const GUID& GUID) const;
			void AddToLoaded(const GUID& GUID, ResourceTypes Type, ResourceBase* Resource);

			template<typename T>
			Resource<T>* AllocateResource(T* ResourcePtr) const
			{
				Resource<T>* handle = ResourceSystemAllocators::ResourceAllocator_Allocate<Resource<T>>();
				Construct(handle, ResourcePtr);

				return handle;
			}

			ResourceTypes GetResourceType(ResourceBase* Resource) const;

			void IOThreadWorker(void);

			void OnResourceCompiled(const GUID& GUID, const WString& FullPath);
			DECLARE_MEMBER_EVENT_LISTENER(ResourceHolder, OnResourceCompiled);

			void FetchShaderSource(const String& Name, String& Source);
			DECLARE_MEMBER_EVENT_LISTENER(ResourceHolder, FetchShaderSource);

		private:
			ResourceCompiler m_Compiler;
			Thread m_IOThread;
			ResourceLoaderTaskQueue m_ResourceLoaderTasks;
			SpinLock m_ResourceLoaderTasksLock;
			WString m_LibraryPath;
			ResourceDatabase* m_ResourceDatabase;
			ResourceMap m_LoadedResources;
			ResourceByNameMap m_WaitingToCompile;
		};
	}
}
#endif