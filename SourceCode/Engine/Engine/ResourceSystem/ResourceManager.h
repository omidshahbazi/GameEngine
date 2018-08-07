// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\ResourceFactory.h>
#include <Containers\Strings.h>

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

				SetLibraryWorkingPath();

				ByteBuffer *buffer = ReadDataFile(GetDataFileName(finalPath));

				if (buffer == nullptr)
					return Resource<T>();

				T *resource = ResourceFactory::GetInstance()->Create<T>(buffer);

				buffer->~Buffer();
				ResourceSystemAllocators::Deallocate(buffer);

				RevertWorkingPath();

				return resource;
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

		private:
			WString m_LastWorkingPath;
		};
	}
}

#endif