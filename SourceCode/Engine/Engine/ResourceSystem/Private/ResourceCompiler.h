// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_COMPILER_H
#define RESOURCE_COMPILER_H

#include <ResourceSystem\Resource.h>
#include <Common\SpinLock.h>
#include <Containers\Strings.h>
#include <Containers\Queue.h>
#include <Containers\Promise.h>
#include <Containers\ListenerContainer.h>
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

				struct CompileTaskInfo
				{
				public:
					CompileTaskInfo(ResourceCompiler* Holder, bool Force, PromiseBlock<void>* PromiseBlock) :
						Holder(Holder),
						Force(Force),
						Promise(PromiseBlock)
					{
					}

					virtual void operator()(void) = 0;

				protected:
					WString GetDataFilePath(const WString& AssetFilePath);

				public:
					ResourceCompiler* Holder;
					bool Force;

					PromiseBlock<void>* Promise;
				};

				struct SingleCompileTaskInfo : public CompileTaskInfo
				{
				public:
					SingleCompileTaskInfo(ResourceCompiler* Holder, bool Force, PromiseBlock<void>* PromiseBlock, const WString& AssetFilePath, FileTypes FileType) :
						CompileTaskInfo(Holder, Force, PromiseBlock),
						AssetFilePath(AssetFilePath),
						FileType(FileType)
					{
					}

					virtual void operator()(void) override;

				public:
					WString AssetFilePath;
					FileTypes FileType;
				};

				struct MultipleCompileTaskInfo : public CompileTaskInfo
				{
				public:
					MultipleCompileTaskInfo(ResourceCompiler* Holder, bool Force, PromiseBlock<void>* PromiseBlock, const WStringList& AssetFilePaths) :
						CompileTaskInfo(Holder, Force, PromiseBlock),
						AssetFilePaths(AssetFilePaths)
					{
					}

					virtual void operator()(void) override;

				public:
					WStringList AssetFilePaths;
				};

				typedef Queue<CompileTaskInfo*> CompileTaskInfoQueue;

				struct ResourceInfo
				{
				public:
					ResourceTypes Type;
					ResourceHandleBase* Resource;
				};

			public:
				class IListener
				{
				public:
					virtual void OnResourceCompiled(const WString& FilePath)
					{
					}
				};

				LISTENER_DECLARATION(IListener)

			public:
				ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath);
				virtual ~ResourceCompiler(void);

				Promise<void> CompileResource(const String& FilePath, bool Force = false)
				{
					return CompileResource(FilePath.ChangeType<char16>(), Force);
				}
				Promise<void> CompileResource(const WString& FilePath, bool Force = false);
				Promise<void> CompileResources(bool Force = false);

				virtual const WString& GetResourcesPath(void) const
				{
					return m_ResourcesPath;
				}

				virtual const WString& GetLibraryPath(void) const
				{
					return m_LibraryPath;
				}

			private:
				void RemoveUnusedMetaFiles(void);

				bool CompileFile(const WString& FilePath, const WString& DataFilePath, FileTypes FileType, bool Force);

				void CheckDirectories(void);

				void IOThreadWorker(void);

				static FileTypes GetFileTypeByExtension(const WString& Extension);

			private:
				Thread m_IOThread;
				CompileTaskInfoQueue m_CompileTasks;
				SpinLock m_CompileTasksLock;
				WString m_ResourcesPath;
				WString m_LibraryPath;
			};
		}
	}
}
#endif