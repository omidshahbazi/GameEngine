// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceCompiler.h>
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
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;

	namespace ResourceSystem
	{
		namespace Private
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

					FileTypes fileType = Compiler->GetFileTypeByExtension(Path::GetExtension(assetFilePath));
					if (fileType == FileTypes::Unknown)
						continue;

					Compiler->CompileFile(assetFilePath, fileType, Force);
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
				ResourceSystemAllocators::ResourceAllocator_Deallocate(m_ResourceDatabase);

				m_IOThread.Shutdown().Wait();
			}

			void ResourceCompiler::Initialize(void)
			{
				m_ResourceDatabase = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceDatabase>();
				Construct(m_ResourceDatabase, m_LibraryPath);

				CheckDirectories();

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceCompiler IO");
			}

			Promise<void> ResourceCompiler::CompileResource(const WString& RelativeFilePath, bool Force)
			{
				FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(RelativeFilePath));

				if (fileType == FileTypes::Unknown)
					return nullptr;

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
				FileSystem::GetFiles(GetResourcesPath(), files, FileSystem::SearchOptions::All);

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

				RemoveUnusedMetaFiles();

				return promiseBlock;
			}

			void ResourceCompiler::RemoveUnusedMetaFiles(void)
			{
				WStringList files;
				FileSystem::GetFiles(GetResourcesPath(), files, ImExporter::META_EXTENSION, FileSystem::SearchOptions::All);

				for (const auto& path : files)
				{
					const auto originalPath = path.SubString(0, path.GetLength() - Path::GetExtension(path).GetLength());

					if (FileSystem::Exists(originalPath))
						continue;

					FileSystem::Delete(path);
				}
			}

			bool ResourceCompiler::CompileFile(const WString& FullPath, FileTypes FileType, bool Force)
			{
				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				bool result = Utilities::ReadDataFile(inBuffer, FullPath);

				if (!result)
					return false;

				FrameAllocator outBufferAllocator("Resource Holder Out Buffer Allocator", ResourceSystemAllocators::ResourceAllocator);
				ByteBuffer outBuffer(&outBufferAllocator, outBufferAllocator.GetReservedSize());

				WString relativeFilePath = Path::GetRelativePath(GetResourcesPath(), FullPath);
				WString dataFullPath;

				bool forceToCompile = Force;

				GUID resourceDatabaseID = m_ResourceDatabase->GetGUID(relativeFilePath);
				if (resourceDatabaseID == GUID::Invalid)
					forceToCompile = true;
				else
				{
					dataFullPath = Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(resourceDatabaseID));

					if (m_ResourceDatabase->CheckDuplicate(resourceDatabaseID, relativeFilePath))
						ImExporter::Invalidate(FullPath);

					if (!forceToCompile)
						forceToCompile = !PlatformFile::Exists(dataFullPath.GetValue());
				}

				GUID resourceID;

				switch (FileType)
				{
				case FileTypes::TXT:
				{
					ImExporter::TextSettings settings;
					if (result = (!ImExporter::ImportText(FullPath, &settings) || forceToCompile))
					{
						result = ImExporter::ExportText(FullPath, &settings);

						if (result)
							result = ResourceFactory::CompileTXT(outBuffer, inBuffer, settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::PNG:
				case FileTypes::JPG:
				{
					ImExporter::TextureSettings settings;
					if (result = (!ImExporter::ImportTexture(FullPath, &settings) || forceToCompile))
					{
						result = ImExporter::ExportTexture(FullPath, &settings);

						if (result)
						{
							if (FileType == FileTypes::PNG)
								result = ResourceFactory::CompilePNG(outBuffer, inBuffer, settings);
							else if (FileType == FileTypes::JPG)
								result = ResourceFactory::CompileJPG(outBuffer, inBuffer, settings);
						}
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::PROGRAM:
				{
					ImExporter::ProgramSettings settings;
					if (result = (!ImExporter::ImportProgram(FullPath, &settings) || forceToCompile))
					{
						result = ImExporter::ExportProgram(FullPath, &settings);

						if (result)
						{
							try
							{
								result = ResourceFactory::CompilePROGRAM(outBuffer, inBuffer, settings);
							}
							catch (const Exception& ex)
							{
								CoreDebugLogError(Categories::ResourceSystem, "[%s] compilation has failed: [%s]", relativeFilePath.ChangeType<char8>().GetValue(), ex.GetInfo().GetValue());
								result = false;
							}
						}
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::OBJ:
				{
					ImExporter::MeshSettings settings;
					if (result = (!ImExporter::ImportMesh(FullPath, &settings) || forceToCompile))
					{
						result = ImExporter::ExportMesh(FullPath, &settings);

						if (result)
							result = ResourceFactory::CompileOBJ(outBuffer, inBuffer, settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::TTF:
				{
					ImExporter::FontSettings settings;
					if (result = (!ImExporter::ImportFont(FullPath, &settings) || forceToCompile))
					{
						result = ImExporter::ExportFont(FullPath, &settings);

						if (result)
							result = ResourceFactory::CompileTTF(outBuffer, inBuffer, settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;
				}

				if (!result)
					return false;

				if (dataFullPath == WString::Empty)
					dataFullPath = Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(resourceID));
				else
					CoreDebugAssert(Categories::ResourceSystem, resourceID == resourceDatabaseID, "Resource [%s] id in database missmatch with the id inside data file", relativeFilePath.GetValue());

				result = Utilities::WriteDataFile(dataFullPath, outBuffer);

				m_ResourceDatabase->AddCompiledResource(relativeFilePath, resourceID);

				OnResourceCompiledEvent(resourceID, relativeFilePath);

				return result;
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
}