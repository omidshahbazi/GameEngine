// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceCompiler.h>
#include <ResourceSystem\Private\Utilities.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceDatabase.h>
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

				Compiler->CompileFile(assetFilePath, Compiler->GetFileTypeByExtension(Path::GetExtension(assetFilePath)), Force);
			}

			Promise->Drop();
		}

		ResourceCompiler::ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
			m_ResourcesPath(ResourcesFullPath),
			m_LibraryPath(LibraryFullPath),
			m_ResourceDatabase(nullptr)
		{
		}

		ResourceCompiler::~ResourceCompiler(void)
		{
			m_IOThread.Shutdown().Wait();
		}

		void ResourceCompiler::Initialize(ResourceDatabase* ResourceDatabase)
		{
			m_ResourceDatabase = ResourceDatabase;

			CheckDirectories();

			m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
			m_IOThread.SetName("ResourceCompiler IO");

			RefreshDatabase();
		}

		Promise<void> ResourceCompiler::CompileResource(const WString& RelativeFilePath, bool Force)
		{
			FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(RelativeFilePath));
			switch (fileType)
			{
			case FileTypes::META:
			case FileTypes::Unknown:
				return nullptr;
			}

			PromiseBlock<void>* promiseBlock = CreatePromiseBlock(1);

			SingleCompileTaskInfo* task = ResourceSystemAllocators::ResourceAllocator_Allocate<SingleCompileTaskInfo>();
			Construct(task, this, Force, promiseBlock, Path::Combine(m_ResourcesPath, RelativeFilePath), fileType);

			m_CompileTasksLock.Lock();
			m_CompileTasks.Enqueue(task);
			m_CompileTasksLock.Release();

			return promiseBlock;
		}

		Promise<void> ResourceCompiler::CompileResources(bool Force)
		{
			WStringList files;
			GetResourcePaths(files);

			PromiseBlock<void>* promiseBlock = nullptr;

			if (files.GetSize() != 0)
			{
				promiseBlock = CreatePromiseBlock(files.GetSize());

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
			// New Resource -> With Without
			// Moved Resource -> With Without
			// Renamed Resource -> With Without
			// Deleted Resource -> With Without
			// Modified Resource

			//*All of them with and without Meta

			WStringList files;

			FileSystem::GetFiles(GetResourcesPath(), files, ImporterExporter::META_EXTENSION, FileSystem::SearchOptions::All);
			for (const auto& file : files)
			{
				const auto resourceFilePath = ImporterExporter::GetResourceFilePath(file);

				if (FileSystem::Exists(resourceFilePath))
					continue;

				FileSystem::Delete(file);

				m_ResourceDatabase->RemoveResourceInfo(Path::GetRelativePath(GetResourcesPath(), resourceFilePath));
			}

			files.Clear();
			GetResourcePaths(files);

			//Vector<ImporterExporter::Settings> settingList(files.GetSize());
			//for (const auto& file : files)
			//{
			//	FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(file));

			//	ImporterExporter::Settings settings = {};
			//	if (!ImporterExporter::Import(file, &settings))
			//	{
			//		WString relativeFilePath = Path::GetRelativePath(GetResourcesPath(), file);

			//		ResourceDatabase::ResourceInfo info;
			//		if (m_ResourceDatabase->GetResourceInfo(relativeFilePath, info))
			//			settings.ID = info.GUID.ToString();

			//		ImporterExporter::Export(file, &settings);
			//	}

			//	settingList.Add(settings);
			//}

			//for (const auto& firstSettings : settingList)
			//{
			//	uint8 foundCount = 0;

			//	for (const auto& secondSettings : settingList)
			//	{
			//		if (firstSettings.ID != secondSettings.ID)
			//			continue;

			//		if (foundCount++ < 1)
			//			continue;

			//		//secondSettings
			//			export is the basic version, should we do it?
			//	}
			//}

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

					if (existsInDatabase)
					{
						if (info.RelativePath != relativeFilePath)
						{
							if (FileSystem::Exists(GetResourceFullPath(info.RelativePath)))
							{

								//it's duplicate
							}
							else
							{
								info.RelativePath = relativeFilePath;
								shouldUpdateDatabase = true;
							}
						}
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

				//if (!existsInDatabase || info.LastWriteTime != PlatformFile::GetLastWriteTime(file.GetValue()))
				if (shouldUpdateDatabase)
				{
					m_ResourceDatabase->UpdateCompiledResource(info);
				}
			}
		}

		bool ResourceCompiler::CompileFile(const WString& FullPath, FileTypes FileType, bool Force)
		{
			ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

			bool result = Utilities::ReadDataFile(FullPath, inBuffer);
			if (!result)
				return false;

			FrameAllocator outBufferAllocator("Resource Holder Out Buffer Allocator", ResourceSystemAllocators::ResourceAllocator, 128 * MegaByte);
			ByteBuffer outBuffer(&outBufferAllocator, outBufferAllocator.GetReservedSize());

			WString relativeFilePath = Path::GetRelativePath(GetResourcesPath(), FullPath);
			WString dataFullPath;

			bool forceToCompile = Force;

			ResourceDatabase::ResourceInfo info = {};
			if (m_ResourceDatabase->GetResourceInfo(relativeFilePath, info))
			{
				dataFullPath = Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(info.GUID));

				forceToCompile |= !PlatformFile::Exists(FullPath.GetValue());
				forceToCompile |= (PlatformFile::GetLastWriteTime(FullPath.GetValue()) != info.LastWriteTime);
				forceToCompile |= !PlatformFile::Exists(dataFullPath.GetValue());
			}
			else
			{
				info.RelativePath = relativeFilePath;

				forceToCompile = true;
			}

#define BEGIN_IMPLEMENT(Type) \
			ImporterExporter::##Type##Settings settings; \
			if (result = (!ImporterExporter::Import(FullPath, &settings) || forceToCompile)) \
			{ \
				result = ImporterExporter::Export(FullPath, &settings); \
				if (result) \
				{ \
					try \
					{

#define END_IMPLEMENT() \
						if (result) \
							id = settings.ID; \
					} \
					catch (const Exception& ex) \
					{ \
						CoreDebugLogError(Categories::ResourceSystem, "[%S] compilation has failed: [%S]", relativeFilePath.ChangeType<char8>(), ex.GetInfo()); \
						result = false; \
					} \
				} \
			}

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

			if (result)
				info.GUID = id;
			else
				return false;

			if (dataFullPath == WString::Empty)
				dataFullPath = Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(info.GUID));

			result = Utilities::WriteDataFile(dataFullPath, outBuffer);

			info.LastWriteTime = PlatformFile::GetLastWriteTime(FullPath.GetValue());
			m_ResourceDatabase->UpdateCompiledResource(info);

			OnResourceCompiledEvent(info.GUID, relativeFilePath);

			return result;

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

		void ResourceCompiler::GetResourcePaths(WStringList& Files)
		{
			FileSystem::GetFiles(GetResourcesPath(), Files, FileSystem::SearchOptions::All);

			Files.RemoveIf([&](auto& item)
				{
					FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(item));

					switch (fileType)
					{
					case FileTypes::META:
					case FileTypes::Unknown:
						return true;
					}

					return false;
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
	}
}