// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>

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
			class ImExporter;

			//TODO: Load assets async
			class RESOURCESYSTEM_API ResourceHolder : Compiler::IListener
			{
				friend class ImExporter;

			private:
				enum class FileTypes
				{
					TXT = 0,
					PNG = 1,
					JPG = 2,
					SHADER = 3,
					OBJ = 4,
					TTF = 5,
					Unknown
				};

				struct ResourceInfo
				{
				public:
					ResourceTypes Type;
					ResourceHandleBase* Resource;
				};

				typedef Map<uint32, ResourceInfo> ResourceMap;

			public:
				ResourceHolder(const WString& AssetsPath, const WString& LibraryPath);
				virtual ~ResourceHolder(void);

				void CheckResources(void);

				template<typename T>
				Resource<T> Load(const String& Path)
				{
					return Load<T>(Path.ChangeType<char16>());
				}

				template<typename T>
				Resource<T> Load(const WString& Path)
				{
					ResourceHandleBase* anyPtr = GetFromLoaded(Path);

					if (anyPtr != nullptr)
						return ReinterpretCast(ResourceHandle<T>*, anyPtr);

					ResourceTypes type = ResourceTypeSpecifier<T>::Type;

					T* resource = LoadInternal<T>(Path);

					ResourceHandle<T>* handle = AllocateResourceHandle(resource);

					if (handle == nullptr)
						return Resource<T>();

					RevertWorkingPath();

					AddToLoaded(Path, ResourceTypeSpecifier<T>::Type, ReinterpretCast(ResourceHandleBase*, handle));

					return handle;
				}

				template<typename T>
				Resource<T> LoadFromMemory(const String& Name, T* Resource)
				{
					return LoadFromMemory<T>(Name.ChangeType<char16>(), Resource);
				}

				template<typename T>
				Resource<T> LoadFromMemory(const WString& Name, T* Resource)
				{
					ResourceHandle<T>* handle = AllocateResourceHandle(Resource);

					AddToLoaded(Name, ResourceTypeSpecifier<T>::Type, ReinterpretCast(ResourceHandleBase*, handle));

					return handle;
				}

				void Reload(const String& Path)
				{
					Reload(Path.ChangeType<char16>());
				}

				void Reload(const WString& Path);

				template<typename T>
				void Unload(Resource<T> Resource)
				{
					UnloadInternal(ResourceTypeSpecifier<T>::Type, *Resource);
				}

				const WString& GetAssetsPath(void) const
				{
					return m_AssetPath;
				}

				const WString& GetLibraryPath(void) const
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

				void CheckAllResources(void);
				void RemoveUnusedMetaFiles(void);

				bool Compile(const WString& FilePath, ResourceTypes& Type);
				bool CompileFile(const WString& FilePath, const WString& DataFilePath, ResourceTypes& Type);

				template<typename T>
				T* LoadInternal(const WString& FilePath)
				{
					WString finalPath = GetDataFileName(FilePath);

					SetLibraryWorkingPath();

					ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

					if (!ReadDataFile(inBuffer, finalPath))
						return nullptr;

					T* resource = ResourceFactory::Create<T>(inBuffer);

					return resource;
				}

				void UnloadInternal(ResourceTypes Type, ResourceHandleBase* Holder);

				void SetAssetsWorkingPath(void);
				void SetLibraryWorkingPath(void);

				ResourceHandleBase* GetFromLoaded(const WString& Name);
				void AddToLoaded(const WString& Name, ResourceTypes Type, ResourceHandleBase* Holder);

				void SetWorkingPath(const WString& Path);
				void RevertWorkingPath(void);

				void CheckDirectories(void);

				WString GetFullPath(const WString& FilePath);

				bool FetchShaderSource(const String& Name, String& Source) override;

				static bool ReadDataFile(ByteBuffer& Buffer, const WString& Path);
				static bool WriteDataFile(const WString& Path, const ByteBuffer& Buffer);

				static WString GetMetaFileName(const WString& FilePath);
				static WString GetDataFileName(const WString& FilePath);

				static FileTypes GetFileTypeByExtension(const WString& Extension);

			private:
				WString m_AssetPath;
				WString m_LibraryPath;
				WString m_LastWorkingPath;
				ResourceMap m_LoadedResources;
			};
		}
	}
}
#endif