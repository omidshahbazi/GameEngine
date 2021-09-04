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
#include <Rendering\Private\ProgramCompiler\Compiler.h>

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
	using namespace Rendering::Private::ProgramCompiler;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceHolder;
			class ResourceDatabase;

			class RESOURCESYSTEM_API ResourceCompiler
			{
				friend class ResourceHolder;

			private:
				enum class FileTypes
				{
					TXT = 0,
					PNG = 1,
					JPG = 2,
					PROGRAM = 3,
					OBJ = 4,
					TTF = 5,
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

			private:
				ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath);
				~ResourceCompiler(void);

				void Initialize(void);

			public:
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

				const ResourceDatabase* GetDatabase(void) const
				{
					return m_ResourceDatabase;
				}

			private:
				void RemoveUnusedMetaFiles(void);

				bool CompileFile(const WString& FullPath, FileTypes FileType, bool Force);

				void CheckDirectories(void);

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
}
#endif