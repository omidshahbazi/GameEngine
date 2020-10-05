// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceCompiler.h>
#include <ResourceSystem\Private\Utilities.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Rendering\Sprite.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace Utility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			PromiseBlock<void>* CreatePromiseBlock(void)
			{
				return AllocatePromiseBlock<void>(ResourceSystemAllocators::ResourceAllocator, [](PromiseBlockBase* Block) { ResourceSystemAllocators::ResourceAllocator_Deallocate(Block); });
			}

			void ResourceCompiler::CompileTaskInfo::operator()(void)
			{
				Holder->CompileFile(AssetFilePath, DataFilePath, FileType, Force);

				PromiseBlock->SetAsDone();

				PromiseBlock->Drop();
			}

			ResourceCompiler::ResourceCompiler(const WString& ResourcesFullPath, const WString& LibraryFullPath)
			{
				m_ResourcesPath = ResourcesFullPath;
				m_LibraryPath = LibraryFullPath;

				CheckDirectories();

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceHolder IO");
			}

			ResourceCompiler::~ResourceCompiler(void)
			{
				m_IOThread.Suspend();
				m_IOThread.Shutdown();
			}

			Promise<void> ResourceCompiler::CompileResource(const WString& FilePath, bool Force)
			{
				return Compile(FilePath, Force);
			}

			Promise<void> ResourceCompiler::CompileResources(bool Force)
			{
				CompileAllResources(Force);

				RemoveUnusedMetaFiles();

				return nullptr;
			}

			void ResourceCompiler::CompileAllResources(bool Force)
			{
				WStringList files;
				FileSystem::GetFiles(GetResourcesPath(), files, FileSystem::SearchOptions::All);

				for each (const auto & path in files)
					CompileResource(path, Force);
			}

			void ResourceCompiler::RemoveUnusedMetaFiles(void)
			{
				WStringList files;
				FileSystem::GetFiles(GetResourcesPath(), files, Utilities::META_EXTENSION, FileSystem::SearchOptions::All);

				for each (const auto & path in files)
				{
					const auto originalPath = path.SubString(0, path.GetLength() - Path::GetExtension(path).GetLength());

					if (FileSystem::Exists(originalPath))
						continue;

					FileSystem::Delete(path);
				}
			}

			Promise<void> ResourceCompiler::Compile(const WString& FilePath, bool Force)
			{
				FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(FilePath));

				if (fileType == FileTypes::Unknown)
					return nullptr;

				PromiseBlock<void>* promiseBlock = CreatePromiseBlock();

				CompileTaskInfo* task = ResourceSystemAllocators::ResourceAllocator_Allocate<CompileTaskInfo>();
				Construct(task, this, FilePath, Path::Combine(GetLibraryPath(), Utilities::GetDataFileName(FilePath.SubString(GetResourcesPath().GetLength() + 1))), fileType, Force, promiseBlock);

				m_IOTasksLock.Lock();
				m_IOTasks.Enqueue(task);
				m_IOTasksLock.Release();

				return promiseBlock;
			}

			bool ResourceCompiler::CompileFile(const WString& FilePath, const WString& DataFilePath, FileTypes FileType, bool Force)
			{
				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				bool result = Utilities::ReadDataFile(inBuffer, FilePath);

				if (!result)
					return false;

				FrameAllocator outBufferAllocator("Resource Holder Out Buffer Allocator", ResourceSystemAllocators::ResourceAllocator);
				ByteBuffer outBuffer(&outBufferAllocator, outBufferAllocator.GetReservedSize());

				bool forceToCompile = Force || !PlatformFile::Exists(DataFilePath.GetValue());

				switch (FileType)
				{
				case FileTypes::TXT:
				{
					ImExporter::TextSettings settings;
					if (result = (ImExporter::ImportText(FilePath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileTXT(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportText(FilePath, &settings);
					}
				} break;

				case FileTypes::PNG:
				case FileTypes::JPG:
				{
					ImExporter::TextureSettings settings;
					if (result = (ImExporter::ImportTexture(FilePath, &settings) || forceToCompile))
					{
						if (FileType == FileTypes::PNG)
							result = ResourceFactory::CompilePNG(outBuffer, inBuffer, settings);
						else if (FileType == FileTypes::JPG)
							result = ResourceFactory::CompileJPG(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportTexture(FilePath, &settings);
					}
				} break;

				case FileTypes::SHADER:
				{
					ImExporter::ShaderSettings settings;
					if (result = (ImExporter::ImportShader(FilePath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileSHADER(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportShader(FilePath, &settings);
					}
				} break;

				case FileTypes::OBJ:
				{
					ImExporter::MeshSettings settings;
					if (result = (ImExporter::ImportMesh(FilePath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileOBJ(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportMesh(FilePath, &settings);
					}
				} break;

				case FileTypes::TTF:
				{
					ImExporter::FontSettings settings;
					if (result = (ImExporter::ImportFont(FilePath, &settings) || forceToCompile))
					{
						result = ResourceFactory::CompileTTF(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportFont(FilePath, &settings);
					}
				} break;
				}

				if (!result)
					return false;

				result = Utilities::WriteDataFile(DataFilePath, outBuffer);

				CALL_CALLBACK(IListener, OnResourceCompiled, FilePath);

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
				while (true)
				{
					PlatformThread::Sleep(1);

					if (!m_IOTasksLock.TryLock())
						continue;

					IOTaskInfo* task = nullptr;
					if (m_IOTasks.GetSize() != 0)
						m_IOTasks.Dequeue(&task);

					m_IOTasksLock.Release();

					if (task == nullptr)
						continue;

					(*task)();

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

				if (Extension == L".shader")
					return FileTypes::SHADER;

				if (Extension == L".obj")
					return FileTypes::OBJ;

				if (Extension == L".ttf")
					return FileTypes::TTF;

				return FileTypes::Unknown;
			}
		}
	}
}