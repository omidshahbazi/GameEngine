// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_COMPILER_H
#define RESOURCE_COMPILER_H

#include <ResourceSystem\Resource.h>
#include <Common\SpinLock.h>
#include <Containers\Strings.h>
#include <Containers\Queue.h>
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Containers;
	using namespace Threading;

	namespace ResourceSystem
	{
		template<typename T>
		class Resource;

		namespace Private
		{
			class ImExporter;

			//TODO: Load assets async
			class RESOURCESYSTEM_API ResourceCompiler
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

				struct IOTaskInfo
				{
				public:
					IOTaskInfo(ResourceCompiler* Holder) :
						Holder(Holder)
					{
					}

					virtual void operator()(void) = 0;

				public:
					ResourceCompiler* Holder;
				};

				struct CompileTaskInfo : public IOTaskInfo
				{
				public:
					CompileTaskInfo(ResourceCompiler* Holder, const WString& AssetFilePath, const WString& DataFilePath, FileTypes FileType) :
						IOTaskInfo(Holder),
						AssetFilePath(AssetFilePath),
						DataFilePath(DataFilePath),
						FileType(FileType)
					{
					}

					virtual void operator()(void) override;

				public:
					WString AssetFilePath;
					WString DataFilePath;
					FileTypes FileType;
				};

				typedef Queue<IOTaskInfo*> IOTaskInfoQueue;

				struct ResourceInfo
				{
				public:
					ResourceTypes Type;
					ResourceHandleBase* Resource;
				};

			public:
				ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath);
				virtual ~ResourceCompiler(void);

				void CompileResource(const String& FilePath, bool Force = false)
				{
					return CompileResource(FilePath.ChangeType<char16>(), Force);
				}
				void CompileResource(const WString& FilePath, bool Force = false);
				void CompileResources(bool Force = false);

				virtual const WString& GetResourcesPath(void) const
				{
					return m_ResourcesPath;
				}

				virtual const WString& GetLibraryPath(void) const
				{
					return m_LibraryPath;
				}

			private:
				void CompileAllResources(bool Force);
				void RemoveUnusedMetaFiles(void);

				bool Compile(const WString& FilePath, ResourceTypes& ResourceType);
				bool CompileFile(const WString& FilePath, const WString& DataFilePath, FileTypes FileType, ResourceTypes& ResourceType);

				void CheckDirectories(void);

				void IOThreadWorker(void);

				static FileTypes GetFileTypeByExtension(const WString& Extension);

			private:
				Thread m_IOThread;
				IOTaskInfoQueue m_IOTasks;
				SpinLock m_IOTasksLock;
				WString m_ResourcesPath;
				WString m_LibraryPath;
			};
		}
	}
}
#endif