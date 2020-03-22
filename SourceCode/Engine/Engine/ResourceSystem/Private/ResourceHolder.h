// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		template<typename T>
		class Resource;

		namespace Private
		{
			class RESOURCESYSTEM_API ResourceHolder
			{
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
					WString finalPath = GetDataFileName(Path);

					ResourceAnyPointer anyPtr = GetFromLoaded(finalPath);

					if (anyPtr != nullptr)
						return ReinterpretCast(ResourceHandle<T>*, anyPtr);

					T* resource = LoadInternal<T>(Path);

					ResourceHandle<T>* handle = AllocateResourceHandle(resource);

					if (handle == nullptr)
						return Resource<T>();

					RevertWorkingPath();

					AddToLoaded(finalPath, ReinterpretCast(ResourceAnyPointer, handle));

					return handle;
				}

				void Reload(const String& Path)
				{
					Reload(Path.ChangeType<char16>());
				}

				void Reload(const WString& Path);

				MeshResource LoadPrimitiveMesh(PrimitiveMeshTypes Type);

				ProgramResource LoadProgram(const String& Name, const String& Source);

				const WString& GetAssetsPath(void) const
				{
					return m_AssetPath;
				}

				const WString& GetLibraryPath(void) const
				{
					return m_LibraryPath;
				}

			private:
				template<typename T>
				ResourceHandle<T>* AllocateResourceHandle(T* Resource) const
				{
					ResourceHandle<T>* handle = ResourceSystemAllocators::Allocate<ResourceHandle<T>>(1);
					Construct(handle, Resource);
					return handle;
				}

				void CompileAll(void);
				bool Compile(const WString& FilePath, ResourceFactory::ResourceTypes& Type);
				bool CompileFile(const WString& FilePath, const WString& DataFilePath, ResourceFactory::ResourceTypes& Type);

				template<typename T>
				T* LoadInternal(const WString& Path)
				{
					WString finalPath = GetDataFileName(Path);

					SetLibraryWorkingPath();

					ByteBuffer inBuffer(&ResourceSystemAllocators::ResourceAllocator);

					if (!ReadDataFile(inBuffer, finalPath))
						return nullptr;

					T* resource = ResourceFactory::GetInstance()->Create<T>(inBuffer);

					return resource;
				}

				void SetAssetsWorkingPath(void);
				void SetLibraryWorkingPath(void);

				void RevertWorkingPath(void);

				void SetWorkingPath(const WString& Path);

				bool ReadDataFile(ByteBuffer& Buffer, const WString& Path);
				bool WriteDataFile(const WString& Path, const ByteBuffer& Buffer);

				WString GetDataFileName(const WString& FilePath);

				ResourceAnyPointer GetFromLoaded(const WString& FinalPath);
				void AddToLoaded(const WString& FinalPath, ResourceAnyPointer Pointer);

				void GetPrimitiveName(PrimitiveMeshTypes Type, WString& Name);

				void CheckDirectories(void);

			private:
				WString m_AssetPath;
				WString m_LibraryPath;
				WString m_LastWorkingPath;
				Map<uint32, ResourceAnyPointer> m_LoadedResources;
			};
		}
	}
}
#endif