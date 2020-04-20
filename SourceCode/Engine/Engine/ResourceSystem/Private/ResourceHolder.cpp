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

namespace Engine
{
	using namespace Utility;
	using namespace Containers;
	using namespace Platform;

	namespace ResourceSystem
	{
		namespace Private
		{
			const WString META_EXTENSION(L".meta");
			const WString DATA_EXTENSION(L".data");

			uint32 GetHash(const WString& Value)
			{
				return Hash::CRC32(Value.ToLower().GetValue(), Value.GetLength() * sizeof(WString::CharType));
			}

			ResourceHolder::ResourceHolder(const WString& AssetsPath, const WString& LibraryPath)
			{
				m_AssetPath = AssetsPath;
				m_LibraryPath = LibraryPath;

				CheckDirectories();
			}

			ResourceHolder::~ResourceHolder(void)
			{
			}

			void ResourceHolder::CheckResources(void)
			{
				CompileAll();
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

					ResourceAnyPointer ptr = m_LoadedResources[hash];

					switch (type)
					{
					case ResourceTypes::Text:
					{
						ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, ptr);

						Text* oldRes = **handle;

						handle->Swap(LoadInternal<Text>(Path));

						ResourceFactory::DestroyText(oldRes);
					} break;

					case ResourceTypes::Texture:
					{
						ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, ptr);

						Texture* oldRes = **handle;

						handle->Swap(LoadInternal<Texture>(Path));

						ResourceFactory::DestroyTexture(oldRes);
					} break;

					case ResourceTypes::Sprite:
					{
						ResourceHandle<Sprite>* handle = ReinterpretCast(ResourceHandle<Sprite>*, ptr);

						Sprite* oldRes = **handle;

						handle->Swap(LoadInternal<Sprite>(Path));

						ResourceFactory::DestroyTexture(oldRes);
					} break;

					case ResourceTypes::Shader:
					{
						ResourceHandle<Shader>* handle = ReinterpretCast(ResourceHandle<Shader>*, ptr);

						Shader* oldRes = **handle;

						handle->Swap(LoadInternal<Shader>(Path));

						ResourceFactory::DestroyShader(oldRes);
					} break;

					case ResourceTypes::Mesh:
					{
						ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, ptr);

						Mesh* oldRes = **handle;

						handle->Swap(LoadInternal<Mesh>(Path));

						ResourceFactory::DestroyMesh(oldRes);
					} break;

					case ResourceTypes::Font:
					{
						ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, ptr);

						Font* oldRes = **handle;

						handle->Swap(LoadInternal<Font>(Path));

						ResourceFactory::DestroyFont(oldRes);
					} break;
					}
				}
			}

			MeshResource ResourceHolder::LoadPrimitiveMesh(PrimitiveMeshTypes Type)
			{
				WString name;
				GetPrimitiveName(Type, name);

				ResourceAnyPointer anyPtr = GetFromLoaded(name);

				if (anyPtr != nullptr)
					return ReinterpretCast(ResourceHandle<Mesh>*, anyPtr);

				Mesh* resource = ResourceFactory::CreatePrimitiveMesh(Type);

				ResourceHandle<Mesh>* handle = AllocateResourceHandle(resource);

				AddToLoaded(name, ReinterpretCast(ResourceAnyPointer, handle));

				return handle;
			}

			ShaderResource ResourceHolder::LoadShader(const String& Name, const String& Source, String* Message)
			{
				Assert(Name.GetLength() != 0, "Name cannot be empty");
				Assert(Source.GetLength() != 0, "Source cannot be empty");

				WString name = Name.ChangeType<char16>();

				ResourceAnyPointer anyPtr = GetFromLoaded(name);

				if (anyPtr != nullptr)
					return ReinterpretCast(ResourceHandle<Shader>*, anyPtr);

				ByteBuffer buffer(ReinterpretCast(byte*, ConstCast(char8*, Source.GetValue())), Source.GetLength());

				Shader* resource = ResourceFactory::CreateShader(buffer, Message);

				ResourceHandle<Shader>* handle = AllocateResourceHandle(resource);

				AddToLoaded(name, ReinterpretCast(ResourceAnyPointer, handle));

				return handle;
			}

			void ResourceHolder::CompileAll(void)
			{
				WString assetsPath = GetAssetsPath();

				Vector<WString> files;
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

			bool ResourceHolder::Compile(const WString& FilePath, ResourceTypes& Type)
			{
				SetAssetsWorkingPath();

				WStringStream dataFilePathStream(&ResourceSystemAllocators::ResourceAllocator);
				dataFilePathStream << GetLibraryPath() << "/" << GetDataFileName(FilePath) << '\0';

				bool result = CompileFile(FilePath, dataFilePathStream.GetBuffer(), Type);

				RevertWorkingPath();

				return result;
			}

			bool ResourceHolder::CompileFile(const WString& FilePath, const WString& DataFilePath, ResourceTypes& Type)
			{
				ByteBuffer inBuffer(&ResourceSystemAllocators::ResourceAllocator);

				bool result = ReadDataFile(inBuffer, FilePath);

				if (!result)
					return false;

				FileTypes fileType = GetFileTypeByExtension(Path::GetExtension(FilePath));

				if (fileType == FileTypes::Unknown)
					return false;

				ByteBuffer outBuffer(&ResourceSystemAllocators::ResourceAllocator);

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

			void ResourceHolder::SetAssetsWorkingPath(void)
			{
				SetWorkingPath(GetAssetsPath());
			}

			void ResourceHolder::SetLibraryWorkingPath(void)
			{
				SetWorkingPath(GetLibraryPath());
			}

			ResourceAnyPointer ResourceHolder::GetFromLoaded(const WString& FinalPath)
			{
				uint32 hash = GetHash(FinalPath);

				if (m_LoadedResources.Contains(hash))
					return m_LoadedResources[hash];

				return nullptr;
			}

			void ResourceHolder::AddToLoaded(const WString& FinalPath, ResourceAnyPointer Pointer)
			{
				uint32 hash = GetHash(FinalPath);

				m_LoadedResources[hash] = Pointer;
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
				WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
				stream << m_AssetPath << '/' << FilePath << '\0';
				return stream.GetBuffer();
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
				WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
				stream << FilePath << META_EXTENSION << '\0';
				return stream.GetBuffer();
			}

			WString ResourceHolder::GetDataFileName(const WString& FilePath)
			{
				WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
				uint32 hash = GetHash(FilePath);
				stream << hash << DATA_EXTENSION << '\0';
				return stream.GetBuffer();
			}

			void ResourceHolder::GetPrimitiveName(PrimitiveMeshTypes Type, WString& Name)
			{
				switch (Type)
				{
				case Engine::ResourceSystem::PrimitiveMeshTypes::Quad:
					Name = L"Quad.mesh";
					break;
				case Engine::ResourceSystem::PrimitiveMeshTypes::Cube:
					Name = L"Cube.mesh";
					break;
				case Engine::ResourceSystem::PrimitiveMeshTypes::Sphere:
					Name = L"Sphere.mesh";
					break;
				}
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