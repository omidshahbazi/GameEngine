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

		public:
			enum class FileTypes
			{
				Unknown = 1 << 0,
				META = 1 << 1,
				TXT = 1 << 2,
				PNG = 1 << 3,
				JPG = 1 << 4,
				PROGRAM = 1 << 5,
				OBJ = 1 << 6,
				TTF = 1 << 7,
				All = 0xFFFFFFF,
				AllResources = All & ~(Unknown | META)
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
			ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath, ResourceDatabase* ResourceDatabase, bool ServeIncludes);
			~ResourceCompiler(void);

			Promise<void> CompileResource(const WString& RelativeFilePath, bool Force = false);
			Promise<void> CompileResource(const String& RelativeFilePath, bool Force = false)
			{
				return CompileResource(RelativeFilePath.ChangeType<char16>(), Force);
			}

			Promise<void> CompileResources(const WString& RelativePath, FileTypes FileTypesMask, bool Force = false);
			Promise<void> CompileResources(const String& RelativePath, bool Force = false)
			{
				return CompileResources(RelativePath.ChangeType<char16>(), FileTypes::AllResources, Force);
			}

			Promise<void> CompileResources(FileTypes FileTypesMask, bool Force = false)
			{
				return CompileResources(WString::Empty, FileTypes::AllResources, Force);
			}
			Promise<void> CompileResources(bool Force = false)
			{
				return CompileResources(FileTypes::AllResources, Force);
			}

			const WString& GetResourcesPath(void) const
			{
				return m_ResourcesPath;
			}

			const WString& GetLibraryPath(void) const
			{
				return m_LibraryPath;
			}

		private:
			void RefreshDatabase(void);

			bool CompileFile(const WString& FullPath, FileTypes FileType, bool Force);

			void CheckDirectories(void);

			WString GetResourceFullPath(const WString& RelativePath);
			WString GetDataFileFullPath(const GUID& GUID);
			void GetResourcePaths(const WString& RelativePath, FileTypes FileTypesMask, WStringList& Files);

			void IOThreadWorker(void);

			static FileTypes GetFileTypeByExtension(const WString& Extension);

			void OnFetchShaderSource(const String& Name, bool& Found, String& Source);
			DECLARE_MEMBER_EVENT_LISTENER(ResourceCompiler, OnFetchShaderSource);

		public:
			ResourceCompiledEventHandler OnResourceCompiledEvent;

		private:
			WString m_ResourcesPath;
			WString m_LibraryPath;
			ResourceDatabase* m_ResourceDatabase;
			bool m_ServeIncludes;
			Thread m_IOThread;
			CompileTaskInfoQueue m_CompileTasks;
			SpinLock m_CompileTasksLock;
		};
	}
}
#endif