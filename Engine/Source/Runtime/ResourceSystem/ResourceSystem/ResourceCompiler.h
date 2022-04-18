// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_COMPILER_H
#define RESOURCE_COMPILER_H

#include <Common\SpinLock.h>
#include <Containers\Strings.h>
#include <Containers\Queue.h>
#include <Containers\Promise.h>
#include <Containers\Delegate.h>
#include <Containers\GUID.h>
#include <Threading\Thread.h>

namespace Engine
{
	namespace ResourceCommon
	{
		template<typename T>
		class Resource;
	}

	using namespace Containers;
	using namespace Threading;
	using namespace ResourceCommon;

	namespace ResourceSystem
	{
		class ResourceHolder;

		namespace Private
		{
			class ResourceDatabase;
		}

		using namespace Private;

		class RESOURCESYSTEM_API ResourceCompiler
		{
			friend class ResourceHolder;

		private:
			enum class FileTypes
			{
				META = 0,
				TXT = 1,
				PNG = 2,
				JPG = 3,
				PROGRAM = 4,
				OBJ = 5,
				TTF = 6,
				Unknown
			};

			struct CompileTaskInfo
			{
			public:
				CompileTaskInfo(ResourceCompiler* Compiler, bool Force, PromiseBlock<void>* PromiseBlock) :
					Compiler(Compiler),
					Force(Force),
					Promise(PromiseBlock)
				{
				}

				virtual ~CompileTaskInfo(void)
				{
				}

				virtual void operator()(void) = 0;

			public:
				ResourceCompiler* Compiler;
				bool Force;

				PromiseBlock<void>* Promise;
			};

			struct SingleCompileTaskInfo : public CompileTaskInfo
			{
			public:
				SingleCompileTaskInfo(ResourceCompiler* Holder, bool Force, PromiseBlock<void>* PromiseBlock, const WString& AssetFullPath, FileTypes FileType) :
					CompileTaskInfo(Holder, Force, PromiseBlock),
					AssetFullPath(AssetFullPath),
					FileType(FileType)
				{
				}

				virtual ~SingleCompileTaskInfo(void)
				{
				}

				virtual void operator()(void) override;

			public:
				WString AssetFullPath;
				FileTypes FileType;
			};

			struct MultipleCompileTaskInfo : public CompileTaskInfo
			{
			public:
				MultipleCompileTaskInfo(ResourceCompiler* Holder, bool Force, PromiseBlock<void>* PromiseBlock, const WStringList& AssetsFullPaths) :
					CompileTaskInfo(Holder, Force, PromiseBlock),
					AssetsFullPaths(AssetsFullPaths)
				{
				}

				virtual ~MultipleCompileTaskInfo(void)
				{
				}

				virtual void operator()(void) override;

			public:
				WStringList AssetsFullPaths;
			};

			typedef Queue<CompileTaskInfo*> CompileTaskInfoQueue;

		public:
			typedef Delegate<const GUID&, const WString&> ResourceCompiledEventHandler;

		public:
			ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath, ResourceDatabase* ResourceDatabase);
			~ResourceCompiler(void);

			Promise<void> CompileResource(const String& RelativeFilePath, bool Force = false)
			{
				return CompileResource(RelativeFilePath.ChangeType<char16>(), Force);
			}
			Promise<void> CompileResource(const WString& RelativeFilePath, bool Force = false);
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
			void RefreshDatabase(void);

			bool CompileFile(const WString& FullPath, FileTypes FileType, bool Force);

			void CheckDirectories(void);

			WString GetResourceFullPath(const WString& RelativePath);
			void GetResourcePaths(WStringList& Files);

			void IOThreadWorker(void);

			static FileTypes GetFileTypeByExtension(const WString& Extension);

		public:
			ResourceCompiledEventHandler OnResourceCompiledEvent;

		private:
			Thread m_IOThread;
			CompileTaskInfoQueue m_CompileTasks;
			SpinLock m_CompileTasksLock;
			WString m_ResourcesPath;
			WString m_LibraryPath;
			ResourceDatabase* m_ResourceDatabase;
		};
	}
}
#endif