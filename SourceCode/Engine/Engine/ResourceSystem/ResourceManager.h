// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\ResourceFactory.h>
#include <ResourceSystem\Private\ResourcePointer.h>
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
			SINGLETON_DEFINITION(ResourceManager)

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
				WString finalPath = Path.ToLower();
				finalPath = GetDataFileName(finalPath);

				ResourceAnyPointer anyPtr = GetFromLoaded(finalPath);

				if (anyPtr != nullptr)
				{
					ResourcePointer<T> *ptr = ReinterpretCast(ResourcePointer<T>*, anyPtr);

					return ptr;
				}

				SetLibraryWorkingPath();

				ByteBuffer *buffer = ReadDataFile(finalPath);

				if (buffer == nullptr)
					return Resource<T>();

				T *resource = ResourceFactory::GetInstance()->Create<T>(buffer);

				buffer->~Buffer();
				ResourceSystemAllocators::Deallocate(buffer);

				RevertWorkingPath();

				ResourcePointer<T> *ptr = ResourceSystemAllocators::Allocate<ResourcePointer<T>>(1);
				Construct(ptr, resource);

				SetToLoaded(finalPath, ReinterpretCast(ResourceAnyPointer, ptr));

				return ptr;
			}

		private:
			void Compile(void);

			bool CompileFile(const WString &FilePath, const WString &DataFilePath);

			bool ProcessFile(const WString &FilePath);

			void SetAssetsWorkingPath(void);
			void SetLibraryWorkingPath(void);

			void RevertWorkingPath(void);

			void SetWorkingPath(const WString &Path);

			WString GetDataFileName(const WString &FilePath);

			ByteBuffer *ReadDataFile(const WString &Path);
			bool WriteDataFile(const WString &Path, ByteBuffer *Buffer);

			ResourceAnyPointer GetFromLoaded(const WString &FinalPath);
			void SetToLoaded(const WString &FinalPath, ResourceAnyPointer Pointer);

		private:
			WString m_LastWorkingPath;
			Map<uint32, ResourceAnyPointer> m_LoadedResources;
		};
	}
}

#endif