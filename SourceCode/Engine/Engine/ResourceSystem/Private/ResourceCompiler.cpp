// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceCompiler.h>
#include <ResourceSystem\Private\Utilities.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Rendering\Sprite.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace Rendering;

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

			ResourceCompiler::ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath)
			{
				m_ResourcesPath = ResourcesFullPath;
				m_LibraryPath = LibraryFullPath;

				Compiler::GetInstance()->AddListener(this);

				CheckDirectories();

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceCompiler IO");
			}

			ResourceCompiler::~ResourceCompiler(void)
			{
				m_IOThread.Shutdown().Wait();

				Compiler::GetInstance()->RemoveListener(this);
			}

			Promise<void> ResourceCompiler::CompileResource(const WString& FullPath, bool Force)
			{
				FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(FullPath));

				if (fileType == FileTypes::Unknown)
					return nullptr;

				PromiseBlock<void>* promiseBlock = CreatePromiseBlock(1);

				SingleCompileTaskInfo* task = ResourceSystemAllocators::ResourceAllocator_Allocate<SingleCompileTaskInfo>();
				Construct(task, this, Force, promiseBlock, FullPath, fileType);

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
				FileSystem::GetFiles(GetResourcesPath(), files, Utilities::META_EXTENSION, FileSystem::SearchOptions::All);

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
				WString dataFullPath = Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(relativeFilePath));

				bool forceToCompile = Force || !PlatformFile::Exists(dataFullPath.GetValue());

				String resourceID;

				switch (FileType)
				{
				case FileTypes::TXT:
				{
					ImExporter::TextSettings settings;
					if (result = (ImExporter::ImportText(FullPath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileTXT(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportText(FullPath, &settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::PNG:
				case FileTypes::JPG:
				{
					ImExporter::TextureSettings settings;
					if (result = (ImExporter::ImportTexture(FullPath, &settings) || forceToCompile))
					{
						if (FileType == FileTypes::PNG)
							result = ResourceFactory::CompilePNG(outBuffer, inBuffer, settings);
						else if (FileType == FileTypes::JPG)
							result = ResourceFactory::CompileJPG(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportTexture(FullPath, &settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::PROGRAM:
				{
					ImExporter::ProgramSettings settings;
					if (result = (ImExporter::ImportProgram(FullPath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompilePROGRAM(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportProgram(FullPath, &settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::OBJ:
				{
					ImExporter::MeshSettings settings;
					if (result = (ImExporter::ImportMesh(FullPath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileOBJ(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportMesh(FullPath, &settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;

				case FileTypes::TTF:
				{
					ImExporter::FontSettings settings;
					if (result = (ImExporter::ImportFont(FullPath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileTTF(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportFont(FullPath, &settings);
					}

					if (result)
						resourceID = settings.ID;
				} break;
				}

				if (!result)
					return false;

				result = Utilities::WriteDataFile(dataFullPath, outBuffer);

				CALL_CALLBACK(IListener, OnResourceCompiled, FullPath, Utilities::GetHash(relativeFilePath), resourceID);

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
						continue;

					(*task)();

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}

				while (m_CompileTasks.GetSize() != 0)
				{
					m_CompileTasks.Dequeue(&task);

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}
			}

			bool ResourceCompiler::FetchShaderSource(const String& Name, String& Source)
			{
				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);
				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(m_ResourcesPath, Name.ChangeType<char16>())))
					return false;

				Source = String(ReinterpretCast(str, inBuffer.GetBuffer()), inBuffer.GetSize());

				return true;
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