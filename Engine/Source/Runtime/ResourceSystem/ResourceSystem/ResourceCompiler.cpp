// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceCompiler.h>
#include <ResourceSystem\Private\Utilities.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <ProgramCompiler\ProgramToAPICompiler.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <RenderSystem\Sprite.h>
#include <Allocators\FrameAllocator.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace ProgramCompiler;

	namespace ResourceSystem
	{
		PromiseBlock<void>* CreatePromiseBlock(uint32 MustDoneCount)
		{
			auto promise = AllocatePromiseBlock<void>(ResourceSystemAllocators::ResourceAllocator, MustDoneCount);
			promise->Grab();
			return promise;
		}

		void ResourceCompiler::SingleCompileTaskInfo::operator()(void)
		{
			Compiler->CompileFile(AssetFullPath, FileType, Force);

			Promise->IncreaseDoneCount();

			Promise->Drop();
		}

		void ResourceCompiler::MultipleCompileTaskInfo::operator()(void)
		{
			for (auto& assetFilePath : AssetsFullPaths)
			{
				Promise->IncreaseDoneCount();

				Compiler->CompileFile(assetFilePath, GetFileTypeByExtension(Path::GetExtension(assetFilePath)), Force);
			}

			Promise->Drop();
		}

		ResourceCompiler::ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath, ResourceDatabase* ResourceDatabase, bool ServeIncludes) :
			m_ResourcesPath(ResourcesFullPath),
			m_LibraryPath(LibraryFullPath),
			m_ResourceDatabase(ResourceDatabase),
			m_ServeIncludes(ServeIncludes)
		{
			CheckDirectories();

			m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
			m_IOThread.SetName("ResourceCompiler IO");

			if (m_ServeIncludes)
				ProgramToAPICompiler::GetInstance()->OnFetchShaderSourceEvent += EventListener_OnFetchShaderSource;

			RefreshDatabase();
		}

		ResourceCompiler::~ResourceCompiler(void)
		{
			if (m_ServeIncludes)
				ProgramToAPICompiler::GetInstance()->OnFetchShaderSourceEvent -= EventListener_OnFetchShaderSource;

			m_IOThread.Shutdown().Wait();
		}

		Promise<void> ResourceCompiler::CompileResource(const WString& RelativeFilePath, bool Force)
		{
			FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(RelativeFilePath));

			if (!BitwiseUtils::IsEnabled(FileTypes::AllResources, fileType))
				return nullptr;

			PromiseBlock<void>* promiseBlock = CreatePromiseBlock(1);

			SingleCompileTaskInfo* task = ResourceSystemAllocators::ResourceAllocator_Allocate<SingleCompileTaskInfo>();
			Construct(task, this, Force, promiseBlock, Path::Combine(m_ResourcesPath, RelativeFilePath), fileType);

			m_CompileTasksLock.Lock();
			m_CompileTasks.Enqueue(task);
			m_CompileTasksLock.Release();

			return promiseBlock;
		}

		Promise<void> ResourceCompiler::CompileResources(const WString& RelativePath, FileTypes FileTypesMask, bool Force)
		{
			WStringList files;
			GetResourcePaths(RelativePath, FileTypesMask, files);

			PromiseBlock<void>* promiseBlock = CreatePromiseBlock(files.GetSize());

			if (files.GetSize() != 0)
			{
				MultipleCompileTaskInfo* task = ResourceSystemAllocators::ResourceAllocator_Allocate<MultipleCompileTaskInfo>();
				Construct(task, this, Force, promiseBlock, files);

				m_CompileTasksLock.Lock();
				m_CompileTasks.Enqueue(task);
				m_CompileTasksLock.Release();
			}

			return promiseBlock;
		}

		void ResourceCompiler::RefreshDatabase(void)
		{
			WStringList files;

			FileSystem::GetFiles(GetResourcesPath(), files, ImporterExporter::META_EXTENSION, FileSystem::SearchOptions::All);
			for (const auto& file : files)
			{
				const auto resourceFilePath = ImporterExporter::GetResourceFilePath(file);

				if (FileSystem::Exists(resourceFilePath))
					continue;

				FileSystem::Delete(file);
			}

			files.Clear();
			GetResourcePaths(WString::Empty, FileTypes::AllResources, files);

			WStringList toKeepInDatabase;
			for (const auto& file : files)
			{
				WString relativeFilePath = Path::GetRelativePath(GetResourcesPath(), file);

				bool existsInDatabase = false;
				bool shouldUpdateDatabase = false;
				ResourceDatabase::ResourceInfo info;

				ImporterExporter::Settings settings = {};
				if (ImporterExporter::Import(file, &settings))
				{
					existsInDatabase = m_ResourceDatabase->GetResourceInfo(settings.ID, info);

					if (existsInDatabase && info.RelativePath != relativeFilePath)
					{
						if (FileSystem::Exists(GetResourceFullPath(info.RelativePath)))
						{
							settings.ID = String::Empty;
							ImporterExporter::Export(file, &settings, false);

							existsInDatabase = false;
							info.LastWriteTime = 0;
						}
						else
							info.RelativePath = relativeFilePath;

						shouldUpdateDatabase = true;
					}
				}
				else
				{
					existsInDatabase = m_ResourceDatabase->GetResourceInfo(relativeFilePath, info);

					if (existsInDatabase)
					{
						settings.ID = info.GUID.ToString();
						shouldUpdateDatabase = true;
					}

					ImporterExporter::Export(file, &settings);
				}

				if (!existsInDatabase)
				{
					info.GUID = settings.ID;
					info.RelativePath = relativeFilePath;
					shouldUpdateDatabase = true;
				}

				if (shouldUpdateDatabase)
					m_ResourceDatabase->UpdateCompiledResource(info);

				toKeepInDatabase.Add(relativeFilePath);
			}

			ResourceDatabase::ResourceInfoList removedResources;
			m_ResourceDatabase->UpdateKeepingResources(toKeepInDatabase, removedResources);

			for (const auto& info : removedResources)
				FileSystem::Delete(GetDataFileFullPath(info.GUID));
		}

		bool ResourceCompiler::CompileFile(const WString& FullPath, FileTypes FileType, bool Force)
		{
			ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

			if (!Utilities::ReadDataFile(FullPath, inBuffer))
				return false;

			FrameAllocator outBufferAllocator("Resource Holder Out Buffer Allocator", ResourceSystemAllocators::ResourceAllocator, 128 * MegaByte);
			ByteBuffer outBuffer(&outBufferAllocator, outBufferAllocator.GetReservedSize());

			WString relativeFilePath = Path::GetRelativePath(GetResourcesPath(), FullPath);
			WString dataFullPath;

			bool forceToCompile = Force;

			ResourceDatabase::ResourceInfo info = {};
			if (m_ResourceDatabase->GetResourceInfo(relativeFilePath, info))
			{
				dataFullPath = GetDataFileFullPath(info.GUID);

				forceToCompile |= !PlatformFile::Exists(FullPath.GetValue());
				forceToCompile |= (PlatformFile::GetLastWriteTime(FullPath.GetValue()) != info.LastWriteTime);
				forceToCompile |= !PlatformFile::Exists(dataFullPath.GetValue());
			}
			else
			{
				info.RelativePath = relativeFilePath;

				forceToCompile = true;
			}

			bool result = false;

#define BEGIN_IMPLEMENT(Type) \
			ImporterExporter::##Type##Settings settings; \
			if (ImporterExporter::Import(FullPath, &settings) && !forceToCompile) \
				return true; \
			if (!ImporterExporter::Export(FullPath, &settings)) \
				return false; \
			try \
			{

#define END_IMPLEMENT() \
				if (result) \
					id = settings.ID; \
			} \
			catch (const Exception& ex) \
			{ \
				CoreDebugLogError(Categories::ResourceSystem, "[%S] compilation has failed: [%S]", relativeFilePath.ChangeType<char8>(), ex.GetInfo()); \
				return false; \
			} \

			String id;
			switch (FileType)
			{
			case FileTypes::TXT:
			{
				BEGIN_IMPLEMENT(Text)
					result = ResourceFactory::CompileTXT(outBuffer, inBuffer, settings);
				END_IMPLEMENT()
			} break;

			case FileTypes::PNG:
			case FileTypes::JPG:
			{
				BEGIN_IMPLEMENT(Texture)
					if (FileType == FileTypes::PNG)
						result = ResourceFactory::CompilePNG(outBuffer, inBuffer, settings);
					else if (FileType == FileTypes::JPG)
						result = ResourceFactory::CompileJPG(outBuffer, inBuffer, settings);
				END_IMPLEMENT()
			} break;

			case FileTypes::PROGRAM:
			{
				BEGIN_IMPLEMENT(Program)
					result = ResourceFactory::CompilePROGRAM(outBuffer, inBuffer, settings);
				END_IMPLEMENT()
			} break;

			case FileTypes::OBJ:
			{
				BEGIN_IMPLEMENT(Mesh)
					result = ResourceFactory::CompileOBJ(outBuffer, inBuffer, settings);
				END_IMPLEMENT()
			} break;

			case FileTypes::TTF:
			{
				BEGIN_IMPLEMENT(Font)
					result = ResourceFactory::CompileTTF(outBuffer, inBuffer, settings);
				END_IMPLEMENT()
			} break;
			}

			if (!result)
				return false;

			info.GUID = id;

			if (dataFullPath == WString::Empty)
				dataFullPath = GetDataFileFullPath(info.GUID);

			if (!Utilities::WriteDataFile(dataFullPath, outBuffer))
				return false;

			info.LastWriteTime = PlatformFile::GetLastWriteTime(FullPath.GetValue());
			m_ResourceDatabase->UpdateCompiledResource(info);

			OnResourceCompiledEvent(info.GUID, relativeFilePath);

			return true;

#undef END_IMPLEMENT
#undef BEGIN_IMPLEMENT
		}

		void ResourceCompiler::CheckDirectories(void)
		{
			WString dir = GetResourcesPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir = GetLibraryPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());
		}

		WString ResourceCompiler::GetResourceFullPath(const WString& RelativePath)
		{
			return Path::Combine(GetResourcesPath(), RelativePath);
		}

		WString ResourceCompiler::GetDataFileFullPath(const GUID& GUID)
		{
			return Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(GUID));
		}

		void ResourceCompiler::GetResourcePaths(const WString& RelativePath, FileTypes FileTypesMask, WStringList& Files)
		{
			FileSystem::GetFiles(GetResourceFullPath(RelativePath), Files, FileSystem::SearchOptions::All);

			Files.RemoveIf([&](auto& item)
				{
					return !BitwiseUtils::IsEnabled(FileTypesMask, GetFileTypeByExtension(Path::GetExtension(item)));
				});
		}

		void ResourceCompiler::IOThreadWorker(void)
		{
			CompileTaskInfo* task = nullptr;

			while (!m_IOThread.GetShouldExit())
			{
				PlatformThread::Sleep(1);

				if (!m_CompileTasksLock.TryLock())
					continue;

				if (m_CompileTasks.GetSize() == 0)
					task = nullptr;
				else
					m_CompileTasks.Dequeue(&task);

				m_CompileTasksLock.Release();

				if (task == nullptr)
				{
					PlatformThread::YieldThread();
					continue;
				}

				(*task)();

				ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
			}

			while (m_CompileTasks.GetSize() != 0)
			{
				m_CompileTasks.Dequeue(&task);

				ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
			}
		}

		ResourceCompiler::FileTypes ResourceCompiler::GetFileTypeByExtension(const WString& Extension)
		{
			if (Extension == L".meta")
				return FileTypes::META;

			if (Extension == L".txt")
				return FileTypes::TXT;

			if (Extension == L".png")
				return FileTypes::PNG;

			if (Extension == L".jpg")
				return FileTypes::JPG;

			if (Extension == L".program")
				return FileTypes::PROGRAM;

			if (Extension == L".obj")
				return FileTypes::OBJ;

			if (Extension == L".ttf")
				return FileTypes::TTF;

			return FileTypes::Unknown;
		}

		void ResourceCompiler::OnFetchShaderSource(const String& RelativeFilePath, bool& Found, String& Source)
		{
			ResourceDatabase::ResourceInfo info = {};
			if (!m_ResourceDatabase->GetResourceInfo(RelativeFilePath.ChangeType<char16>(), info))
				return;

			WString dataFilePath = GetDataFileFullPath(info.GUID);

			if (FileSystem::Exists(dataFilePath))
				THROW_EXCEPTION(Categories::ResourceSystem, "The program has supposed to be compiled and must got catched in the prior handler of this event in ResourceHolder");

			if (!CompileFile(GetResourceFullPath(RelativeFilePath.ChangeType<char16>()), FileTypes::PROGRAM, true))
				return;

			ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);
			if (!Utilities::ReadDataFile(dataFilePath, inBuffer))
				return;

			Found = true;
			Source = ResourceFactory::GetProgramSource(inBuffer);
		}
	}
}