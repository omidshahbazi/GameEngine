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

	namespace ResourceSystem
	{
		template<typename T>
		class Resource;

		namespace Private
		{
			//TODO: Load assets async
			class RESOURCESYSTEM_API ResourceHolder : Compiler::IListener
			{
			private:
				struct ResourceInfo
				{
				public:
					ResourceTypes Type;
					ResourceHandleBase* Resource;
				};

				typedef Map<uint32, ResourceInfo> ResourceMap;

			public:
				ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath);
				virtual ~ResourceHolder(void);

				template<typename T>
				Resource<T> Load(const String& FilePath)
				{
					return Load<T>(FilePath.ChangeType<char16>());
				}

				template<typename T>
				Resource<T> Load(const WString& FilePath)
				{
					ResourceHandleBase* anyPtr = GetFromLoaded(FilePath);

					if (anyPtr != nullptr)
						return ReinterpretCast(ResourceHandle<T>*, anyPtr);

					T* resource = LoadInternal<T>(FilePath);

					if (resource == nullptr)
						return Resource<T>();

					ResourceHandle<T>* handle = AllocateResourceHandle(resource);

					AddToLoaded(FilePath, ResourceTypeSpecifier<T>::Type, handle);

					return handle;
				}

				template<typename T>
				Resource<T> AddFromMemory(const String& Name, T* Resource)
				{
					return AddFromMemory<T>(Name.ChangeType<char16>(), Resource);
				}

				template<typename T>
				Resource<T> AddFromMemory(const WString& Name, T* Resource)
				{
					ResourceHandle<T>* handle = AllocateResourceHandle(Resource);

					AddToLoaded(Name, ResourceTypeSpecifier<T>::Type, handle);

					return handle;
				}

				//TODO: fill this
				//template<typename T>
				//Resource<T> LoadFromMemory(const String& Name, ??????????)
				//{
				//	return LoadFromMemory<T>(Name.ChangeType<char16>(), Resource);
				//}

				//template<typename T>
				//Resource<T> LoadFromMemory(const WString& Name, ? ? ? ? ? ? ? ? ? ? )
				//{
				//	ResourceHandle<T>* handle = AllocateResourceHandle(Resource);

				//	AddToLoaded(Name, ResourceTypeSpecifier<T>::Type, handle);

				//	return handle;
				//}

				//void Reload(const String& FilePath)
				//{
				//	Reload(FilePath.ChangeType<char16>());
				//}

				//void Reload(const WString& FilePath);

				template<typename T>
				void Unload(Resource<T> Resource)
				{
					UnloadInternal(ResourceTypeSpecifier<T>::Type, *Resource);
				}

				virtual ResourceCompiler& GetCompiler(void)
				{
					return m_Compiler;
				}

				virtual const WString& GetLibraryPath(void) const
				{
					return m_LibraryPath;
				}

			protected:
				template<typename T>
				Resource<T> GetLoaded(const String& Name)
				{
					return GetLoaded<T>(Name.ChangeType<char16>());
				}

				template<typename T>
				Resource<T> GetLoaded(const WString& Name)
				{
					return ReinterpretCast(ResourceHandle<T>*, GetFromLoaded(Name));
				}

			private:
				template<typename T>
				ResourceHandle<T>* AllocateResourceHandle(T* Resource) const
				{
					ResourceHandle<T>* handle = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceHandle<T>>();
					Construct(handle, Resource);
					return handle;
				}

				void DeallocateResourceHandle(ResourceHandleBase* Resource) const
				{
					ResourceSystemAllocators::ResourceAllocator_Deallocate(Resource);
				}

				template<typename T>
				T* LoadInternal(const WString& FilePath)
				{
					WString finalPath = Utilities::GetDataFileName(FilePath);

					ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

					if (!Utilities::ReadDataFile(inBuffer, Path::Combine(GetLibraryPath(), finalPath)))
						return nullptr;

					return ResourceFactory::Create<T>(inBuffer);
				}

				void UnloadInternal(ResourceTypes Type, ResourceHandleBase* Holder);

				ResourceHandleBase* GetFromLoaded(const WString& Name);
				void AddToLoaded(const WString& Name, ResourceTypes Type, ResourceHandleBase* Holder);

				bool FetchShaderSource(const String& Name, String& Source) override;

			private:
				ResourceCompiler m_Compiler;
				WString m_LibraryPath;
				ResourceMap m_LoadedResources;
			};
		}
	}
}
#endif