// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
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
		using namespace Private;

		template<typename T>
		class Resource;

		class RESOURCESYSTEM_API ResourceManager
		{
			SINGLETON_DECLARATION(ResourceManager)

		public:
			ResourceManager(void);
			~ResourceManager(void);

			template<typename T>
			Resource<T> Load(const String &Path)
			{
				return Load<T>(Path.ChangeType<char16>());
			}

			template<typename T>
			Resource<T> Load(const WString &Path)
			{
				WString finalPath = GetDataFileName(Path);

				ResourceAnyPointer anyPtr = GetFromLoaded(finalPath);

				if (anyPtr != nullptr)
					return ReinterpretCast(ResourceHandle<T>*, anyPtr);

				T *resource = LoadInternal<T>(Path);

				ResourceHandle<T> *handle = AllocateResourceHandle(resource);

				if (handle == nullptr)
					return Resource<T>();

				RevertWorkingPath();

				SetToLoaded(finalPath, ReinterpretCast(ResourceAnyPointer, handle));

				return handle;
			}

			void Reload(const String &Path)
			{
				Reload(Path.ChangeType<char16>());
			}

			void Reload(const WString &Path);

			MeshResource Load(PrimitiveMeshTypes Type);

			ProgramResource GetDefaultProgram(void);

			const WString &GetAssetsPath(void) const;

			const WString &GetLibraryPath(void) const;

			template<typename T>
			ResourceHandle<T> *AllocateResourceHandle(T *Resource) const
			{
				ResourceHandle<T> *handle = ResourceSystemAllocators::Allocate<ResourceHandle<T>>(1);
				Construct(handle, Resource);
				return handle;
			}

		private:
			void CompileAll(void);
			bool Compile(const WString &FilePath, ResourceFactory::ResourceTypes &Type);
			bool CompileFile(const WString &FilePath, const WString &DataFilePath, ResourceFactory::ResourceTypes &Type);

			template<typename T>
			T *LoadInternal(const WString &Path)
			{
				WString finalPath = GetDataFileName(Path);

				SetLibraryWorkingPath();

				ByteBuffer *buffer = ReadDataFile(finalPath);

				if (buffer == nullptr)
					return nullptr;

				T *resource = ResourceFactory::GetInstance()->Create<T>(buffer);

				buffer->~Buffer();
				ResourceSystemAllocators::Deallocate(buffer);

				return resource;
			}

			void SetAssetsWorkingPath(void);
			void SetLibraryWorkingPath(void);

			void RevertWorkingPath(void);

			void SetWorkingPath(const WString &Path);

			WString GetDataFileName(const WString &FilePath);

			ByteBuffer *ReadDataFile(const WString &Path);
			bool WriteDataFile(const WString &Path, ByteBuffer *Buffer);

			ResourceAnyPointer GetFromLoaded(const WString &FinalPath);
			void SetToLoaded(const WString &FinalPath, ResourceAnyPointer Pointer);

			void GetPrimitiveName(PrimitiveMeshTypes Type, WString &Name);

			void CreateDefaultResources(void);
			void CreateDefaultProgram(void);

		private:
			WString m_LastWorkingPath;
			Map<uint32, ResourceAnyPointer> m_LoadedResources;
		};
	}
}

#endif