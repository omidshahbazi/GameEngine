// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Common\BitwiseUtils.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\Hash.h>
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
			cwstr META_EXTENSION(L".meta");
			cwstr DATA_EXTENSION(L".data");

			uint32 GetHash(const WString& Value)
			{
				WString value = Value.Replace(L"/", L"\\");
				value = value.ToLower();

				return Hash::CRC32(value.GetValue(), value.GetLength() * sizeof(WString::CharType));
			}

			ResourceHolder::ResourceHolder(const WString& AssetsPath, const WString& LibraryPath)
			{
				m_AssetPath = AssetsPath;
				m_LibraryPath = LibraryPath;

				CheckDirectories();

				Compiler::GetInstance()->AddListener(this);
			}

			ResourceHolder::~ResourceHolder(void)
			{
				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					UnloadInternal(info.Type, info.Resource);

					DeallocateResourceHandle(info.Resource);
				}

				m_LoadedResources.Clear();

				Compiler::GetInstance()->RemoveListener(this);
			}

			void ResourceHolder::CheckResources(void)
			{
				CheckAllResources();

				RemoveUnusedMetaFiles();
			}

			void ResourceHolder::Reload(const WString& Path)
			{
				if (Path.EndsWith(META_EXTENSION))
					return;

				uint32 hash = GetHash(GetDataFileName(Path));

				if (m_LoadedResources.Contains(hash))
				{
					ResourceTypes type;
					Compile(Path, type);

					ResourceHandleBase* ptr = m_LoadedResources[hash].Resource;

					ResourceHandleBase oldRes = *ptr;

					switch (type)
					{
					case ResourceTypes::Text:
					{
						ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, ptr);

						handle->Swap(LoadInternal<Text>(Path));
					} break;

					case ResourceTypes::Texture:
					{
						ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, ptr);

						handle->Swap(LoadInternal<Texture>(Path));
					} break;

					case ResourceTypes::Sprite:
					{
						ResourceHandle<Sprite>* handle = ReinterpretCast(ResourceHandle<Sprite>*, ptr);

						handle->Swap(LoadInternal<Sprite>(Path));
					} break;

					case ResourceTypes::Shader:
					{
						ResourceHandle<Shader>* handle = ReinterpretCast(ResourceHandle<Shader>*, ptr);

						handle->Swap(LoadInternal<Shader>(Path));
					} break;

					case ResourceTypes::Mesh:
					{
						ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, ptr);

						handle->Swap(LoadInternal<Mesh>(Path));
					} break;

					case ResourceTypes::Font:
					{
						ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, ptr);

						handle->Swap(LoadInternal<Font>(Path));
					} break;
					}

					UnloadInternal(type, &oldRes);
				}
			}

			void ResourceHolder::CheckAllResources(void)
			{
				WString assetsPath = GetAssetsPath();

				WStringList files;
				FileSystem::GetFiles(assetsPath, files, FileSystem::SearchOptions::All);

				for each (const auto & path in files)
				{
					const auto ext = Path::GetExtension(path).ToLower();

					if (ext == META_EXTENSION || ext == DATA_EXTENSION)
						continue;

					WString finalPath = path.SubString(assetsPath.GetLength() + 1).ToLower();

					ResourceTypes type;
					Compile(finalPath, type);
				}
			}

			void ResourceHolder::RemoveUnusedMetaFiles(void)
			{
				WStringList files;
				FileSystem::GetFiles(GetAssetsPath(), files, META_EXTENSION, FileSystem::SearchOptions::All);

				for each (const auto & path in files)
				{
					const auto originalPath = path.SubString(0, path.GetLength() - Path::GetExtension(path).GetLength());

					if (FileSystem::Exists(originalPath))
						continue;

					FileSystem::Delete(path);
				}
			}

			bool ResourceHolder::Compile(const WString& FilePath, ResourceTypes& Type)
			{
				SetAssetsWorkingPath();

				WStringStream dataFilePathStream(ResourceSystemAllocators::ResourceAllocator);
				dataFilePathStream << GetLibraryPath() << "/" << GetDataFileName(FilePath) << '\0';

				bool result = CompileFile(FilePath, dataFilePathStream.GetBuffer(), Type);

				RevertWorkingPath();

				return result;
			}

			bool ResourceHolder::CompileFile(const WString& FilePath, const WString& DataFilePath, ResourceTypes& Type)
			{
				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				bool result = ReadDataFile(inBuffer, FilePath);

				if (!result)
					return false;

				FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(FilePath));

				if (fileType == FileTypes::Unknown)
					return false;

				FrameAllocator outBufferAllocator("Resource Holder Out Buffer Allocator", ResourceSystemAllocators::ResourceAllocator);
				ByteBuffer outBuffer(&outBufferAllocator, outBufferAllocator.GetReservedSize());

				const WString fullAssetFilePath = GetFullPath(FilePath);

				switch (fileType)
				{
				case FileTypes::TXT:
				{
					ImExporter::TextSettings settings;
					if (result = ImExporter::ImportText(fullAssetFilePath, &settings))
					{
						Type = ResourceTypes::Text;
						result = ResourceFactory::CompileTXT(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportText(fullAssetFilePath, &settings);
					}
				} break;

				case FileTypes::PNG:
				case FileTypes::JPG:
				{
					ImExporter::TextureSettings settings;
					if (result = ImExporter::ImportTexture(fullAssetFilePath, &settings))
					{
						Type = (settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite);

						if (fileType == FileTypes::PNG)
							result = ResourceFactory::CompilePNG(outBuffer, inBuffer, settings);
						else if (fileType == FileTypes::JPG)
							result = ResourceFactory::CompileJPG(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportTexture(fullAssetFilePath, &settings);
					}
				} break;

				case FileTypes::SHADER:
				{
					ImExporter::ShaderSettings settings;
					if (result = ImExporter::ImportShader(fullAssetFilePath, &settings))
					{
						Type = ResourceTypes::Shader;
						result = ResourceFactory::CompileSHADER(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportShader(fullAssetFilePath, &settings);
					}
				} break;

				case FileTypes::OBJ:
				{
					ImExporter::MeshSettings settings;
					if (result = ImExporter::ImportMesh(fullAssetFilePath, &settings))
					{
						Type = ResourceTypes::Mesh;
						result = ResourceFactory::CompileOBJ(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportMesh(fullAssetFilePath, &settings);
					}
				} break;

				case FileTypes::TTF:
				{
					ImExporter::FontSettings settings;
					if (result = ImExporter::ImportFont(fullAssetFilePath, &settings))
					{
						Type = ResourceTypes::Font;
						result = ResourceFactory::CompileTTF(outBuffer, inBuffer, settings);

						if (result)
							result = ImExporter::ExportFont(fullAssetFilePath, &settings);
					}
				} break;
				}

				if (!result)
					return false;

				result = WriteDataFile(DataFilePath, outBuffer);

				return result;
			}

			void ResourceHolder::UnloadInternal(ResourceTypes Type, ResourceHandleBase* Holder)
			{
				switch (Type)
				{
				case ResourceTypes::Text:
				{
					ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, Holder);

					ResourceFactory::DestroyText(**handle);
				} break;

				case ResourceTypes::Texture:
				{
					ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, Holder);

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Sprite:
				{
					ResourceHandle<Sprite>* handle = ReinterpretCast(ResourceHandle<Sprite>*, Holder);

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Shader:
				{
					ResourceHandle<Shader>* handle = ReinterpretCast(ResourceHandle<Shader>*, Holder);

					ResourceFactory::DestroyShader(**handle);
				} break;

				case ResourceTypes::Mesh:
				{
					ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, Holder);

					ResourceFactory::DestroyMesh(**handle);
				} break;

				case ResourceTypes::Font:
				{
					ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, Holder);

					ResourceFactory::DestroyFont(**handle);
				} break;
				}
			}

			void ResourceHolder::SetAssetsWorkingPath(void)
			{
				SetWorkingPath(GetAssetsPath());
			}

			void ResourceHolder::SetLibraryWorkingPath(void)
			{
				SetWorkingPath(GetLibraryPath());
			}

			ResourceHandleBase* ResourceHolder::GetFromLoaded(const WString& Name)
			{
				uint32 hash = GetHash(Name);

				if (m_LoadedResources.Contains(hash))
					return m_LoadedResources[hash].Resource;

				return nullptr;
			}

			void ResourceHolder::AddToLoaded(const WString& Name, ResourceTypes Type, ResourceHandleBase* Holder)
			{
				uint32 hash = GetHash(Name);

				m_LoadedResources[hash] = { Type, Holder };
			}

			void ResourceHolder::RevertWorkingPath(void)
			{
			}

			void ResourceHolder::SetWorkingPath(const WString& Path)
			{
				cwstr currentWorkingPathStr;
				PlatformDirectory::GetWokringDirectory(&currentWorkingPathStr);
				m_LastWorkingPath = currentWorkingPathStr;
				PlatformDirectory::SetWokringDirectory(Path.GetValue());
			}

			void ResourceHolder::CheckDirectories(void)
			{
				WString dir = GetAssetsPath();
				if (!PlatformDirectory::Exists(dir.GetValue()))
					PlatformDirectory::Create(dir.GetValue());

				dir = GetLibraryPath();
				if (!PlatformDirectory::Exists(dir.GetValue()))
					PlatformDirectory::Create(dir.GetValue());
			}

			WString ResourceHolder::GetFullPath(const WString& FilePath)
			{
				WStringStream stream(ResourceSystemAllocators::ResourceAllocator);
				stream << m_AssetPath << '/' << FilePath << '\0';
				return stream.GetBuffer();
			}

			bool ResourceHolder::FetchShaderSource(const String& Name, String& Source)
			{
				//TODO: Need to read from data file not original resource

				const WString path = GetFullPath(Name.ChangeType<char16>());

				if (!FileSystem::Exists(path.GetValue()))
					return false;

				return FileSystem::ReadAllText(path, &Source);
			}

			bool ResourceHolder::ReadDataFile(ByteBuffer& Buffer, const WString& Path)
			{
				auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input | PlatformFile::OpenModes::Binary);

				if (handle == 0)
					return false;

				uint64 fileSize = PlatformFile::Size(handle);

				Buffer.Extend(fileSize);

				if ((fileSize = PlatformFile::Read(handle, Buffer.GetBuffer(), fileSize)) == 0)
					return false;

				PlatformFile::Close(handle);

				return true;
			}

			bool ResourceHolder::WriteDataFile(const WString& Path, const ByteBuffer& Buffer)
			{
				auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);

				if (handle == 0)
					return false;

				PlatformFile::Write(handle, Buffer.GetBuffer(), Buffer.GetSize());

				PlatformFile::Close(handle);

				return true;
			}

			WString ResourceHolder::GetMetaFileName(const WString& FilePath)
			{
				WStringStream stream(ResourceSystemAllocators::ResourceAllocator);
				stream << FilePath << META_EXTENSION << '\0';
				return stream.GetBuffer();
			}

			WString ResourceHolder::GetDataFileName(const WString& FilePath)
			{
				WStringStream stream(ResourceSystemAllocators::ResourceAllocator);
				uint32 hash = GetHash(FilePath);
				stream << hash << DATA_EXTENSION << '\0';
				return stream.GetBuffer();
			}

			ResourceHolder::FileTypes ResourceHolder::GetFileTypeByExtension(const WString& Extension)
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