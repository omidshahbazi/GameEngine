// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Common\BitwiseUtils.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Platform\PlatformOS.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\Hash.h>
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>

namespace Engine
{
	using namespace Utility;
	using namespace Containers;
	using namespace Utility::YAML;
	using namespace Platform;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");
		const WString META_EXTENSION(L".meta");
		const WString DATA_EXTENSION(L".data");
		const String KEY_GUID("GUID");
		const String KEY_LAST_WRITE_TIME("LastWriteTime");
		const String KEY_FILE_FORMAT_VERSION("FileFormatVersion");

		const WString DEFAULT_SHADER_NAME(L"Default.shader");
		String DEFAULT_SHADER_SOURCE("float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");

		const int8 FILE_FORMAT_VERSION = 1;

		const WString &GetAssetsPathInternal(void)
		{
			static bool initialized = false;
			static WString path;

			if (!initialized)
			{
				path = Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME);
				initialized = true;
			}

			return path;
		}

		const WString &GetLibraryPathInternal(void)
		{
			static bool initialized = false;
			static WString path;

			if (!initialized)
			{
				path = Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME);
				initialized = true;
			}

			return path;
		}

		void CheckDirectories(void)
		{
			WString dir = GetAssetsPathInternal();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir = GetLibraryPathInternal();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());
		}

		const String &GenerateUUID(void)
		{
			str uuid;
			PlatformOS::GenerateGUID(&uuid);

			static String result;
			result = uuid;

			return result;
		}

		void ReadMetaFile(const WString &Path, YAMLObject &Object)
		{
			YAMLParser parser;

			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			static char8 str[1024];
			PlatformFile::Read(handle, str, 1024);

			PlatformFile::Close(handle);

			parser.Parse(str, Object);
		}

		void WriteMetaFile(const WString &Path, YAMLObject &Object)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output);
			PlatformFile::Write(handle, Object.ToString().GetValue());
			PlatformFile::Close(handle);
		}

		uint32 GetHash(const WString &Value)
		{
			return Hash::CRC32(Value.ToLower().GetValue(), Value.GetLength() * sizeof(WString::CharType));
		}

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			ResourceFactory::Create(&ResourceSystemAllocators::ResourceAllocator);

			CheckDirectories();

			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		void ResourceManager::CheckResources(void)
		{
			CompileAll();
		}

		void ResourceManager::Reload(const WString & Path)
		{
			if (Path.EndsWith(META_EXTENSION))
				return;

			uint32 hash = GetHash(GetDataFileName(Path));

			if (m_LoadedResources.Contains(hash))
			{
				ResourceFactory::ResourceTypes type;
				Compile(Path, type);

				ResourceAnyPointer ptr = m_LoadedResources[hash];

				switch (type)
				{
				case ResourceFactory::ResourceTypes::Text:
				{
					ResourceHandle<Text> *handle = ReinterpretCast(ResourceHandle<Text>*, ptr);

					Text *oldRes = **handle;

					handle->Swap(LoadInternal<Text>(Path));

					ResourceFactory::GetInstance()->DestroyText(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Texture:
				{
					ResourceHandle<Texture> *handle = ReinterpretCast(ResourceHandle<Texture>*, ptr);

					Texture *oldRes = **handle;

					handle->Swap(LoadInternal<Texture>(Path));

					ResourceFactory::GetInstance()->DestroyTexture(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Shader:
				{
					ResourceHandle<Program> *handle = ReinterpretCast(ResourceHandle<Program>*, ptr);

					Program *oldRes = **handle;

					handle->Swap(LoadInternal<Program>(Path));

					ResourceFactory::GetInstance()->DestroyProgram(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Model:
				{
					ResourceHandle<Mesh> *handle = ReinterpretCast(ResourceHandle<Mesh>*, ptr);

					Mesh *oldRes = **handle;

					handle->Swap(LoadInternal<Mesh>(Path));

					ResourceFactory::GetInstance()->DestroyMesh(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Font:
				{
					ResourceHandle<Font> *handle = ReinterpretCast(ResourceHandle<Font>*, ptr);

					Font *oldRes = **handle;

					handle->Swap(LoadInternal<Font>(Path));

					ResourceFactory::GetInstance()->DestroyFont(oldRes);
				} break;
				}
			}
		}

		MeshResource ResourceManager::Load(PrimitiveMeshTypes Type)
		{
			WString name;
			GetPrimitiveName(Type, name);

			ResourceAnyPointer anyPtr = GetFromLoaded(name);

			if (anyPtr != nullptr)
				return ReinterpretCast(ResourceHandle<Mesh>*, anyPtr);

			Mesh *resource = ResourceFactory::GetInstance()->Create(Type);

			ResourceHandle<Mesh> *handle = AllocateResourceHandle(resource);

			SetToLoaded(name, ReinterpretCast(ResourceAnyPointer, handle));

			return handle;
		}

		ProgramResource ResourceManager::GetDefaultProgram(void)
		{
			return Load<Program>(DEFAULT_SHADER_NAME);
		}

		const WString & ResourceManager::GetAssetsPath(void) const
		{
			return GetAssetsPathInternal();
		}

		const WString & ResourceManager::GetLibraryPath(void) const
		{
			return GetLibraryPathInternal();
		}

		void ResourceManager::CompileAll(void)
		{
			WString assetsPath = GetAssetsPath();

			Vector<WString> files;
			FileSystem::GetFiles(assetsPath, files, FileSystem::SearchOptions::All);

			for each (const auto &path in files)
			{
				if (Path::GetExtension(path).ToLower() == META_EXTENSION)
					continue;

				WString finalPath = path.SubString(assetsPath.GetLength() + 1).ToLower();

				ResourceFactory::ResourceTypes type;
				Compile(finalPath, type);
			}
		}

		bool ResourceManager::Compile(const WString &FilePath, ResourceFactory::ResourceTypes &Type)
		{
			SetAssetsWorkingPath();

			WString metaFilePath = FilePath + META_EXTENSION;
			int64 lastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue());

			WStringStream dataFilePathStream(&ResourceSystemAllocators::ResourceAllocator);
			dataFilePathStream << "../" << LIBRARY_DIRECTORY_NAME << "/" << GetDataFileName(FilePath) << '\0';
			WString dataFilePath = dataFilePathStream.GetBuffer();

			YAMLObject obj;

			if (PlatformFile::Exists(metaFilePath.GetValue()) && PlatformFile::Exists(dataFilePath.GetValue()))
			{
				ReadMetaFile(metaFilePath, obj);

				if (lastWriteTime == obj[KEY_LAST_WRITE_TIME].GetAsInt64())
					return true;
			}
			else
				obj[KEY_GUID] = GenerateUUID();

			obj[KEY_FILE_FORMAT_VERSION] = FILE_FORMAT_VERSION;
			obj[KEY_LAST_WRITE_TIME] = lastWriteTime;

			if (!CompileFile(FilePath, dataFilePath, Type))
				return false;

			WriteMetaFile(metaFilePath, obj);

			RevertWorkingPath();

			return true;
		}

		bool ResourceManager::CompileFile(const WString &FilePath, const WString &DataFilePath, ResourceFactory::ResourceTypes &Type)
		{
			ByteBuffer inBuffer(&ResourceSystemAllocators::ResourceAllocator);

			bool result = ReadDataFile(inBuffer, FilePath);

			if (!result)
				return false;

			ByteBuffer outBuffer(&ResourceSystemAllocators::ResourceAllocator);

			result = ResourceFactory::GetInstance()->Compile(Path::GetExtension(FilePath), outBuffer, inBuffer, Type);

			if (!result)
				return false;

			result = WriteDataFile(DataFilePath, outBuffer);

			return result;
		}

		void ResourceManager::SetAssetsWorkingPath(void)
		{
			SetWorkingPath(GetAssetsPath());
		}

		void ResourceManager::SetLibraryWorkingPath(void)
		{
			SetWorkingPath(GetLibraryPath());
		}

		void ResourceManager::RevertWorkingPath(void)
		{
		}

		void ResourceManager::SetWorkingPath(const WString & Path)
		{
			cwstr currentWorkingPathStr;
			PlatformDirectory::GetWokringDirectory(&currentWorkingPathStr);
			m_LastWorkingPath = currentWorkingPathStr;
			PlatformDirectory::SetWokringDirectory(Path.GetValue());
		}

		bool ResourceManager::ReadDataFile(ByteBuffer &Buffer, const WString &Path)
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

		bool ResourceManager::WriteDataFile(const WString &Path, const ByteBuffer &Buffer)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);

			if (handle == 0)
				return false;

			PlatformFile::Write(handle, Buffer.GetBuffer(), Buffer.GetSize());

			PlatformFile::Close(handle);

			return true;
		}

		WString ResourceManager::GetDataFileName(const WString &FilePath)
		{
			WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
			uint32 hash = GetHash(FilePath);
			stream << hash << DATA_EXTENSION << '\0';
			return stream.GetBuffer();
		}

		ResourceAnyPointer ResourceManager::GetFromLoaded(const WString &FinalPath)
		{
			uint32 hash = GetHash(FinalPath);

			if (m_LoadedResources.Contains(hash))
				return m_LoadedResources[hash];

			return nullptr;
		}

		void ResourceManager::SetToLoaded(const WString & FinalPath, ResourceAnyPointer Pointer)
		{
			uint32 hash = GetHash(FinalPath);

			m_LoadedResources[hash] = Pointer;
		}

		void ResourceManager::GetPrimitiveName(PrimitiveMeshTypes Type, WString & Name)
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

		void ResourceManager::CreateDefaultResources(void)
		{
			CreateDefaultProgram();
		}

		void ResourceManager::CreateDefaultProgram(void)
		{
			ByteBuffer buffer(ReinterpretCast(byte*, DEFAULT_SHADER_SOURCE.GetValue()), DEFAULT_SHADER_SOURCE.GetLength());
			Program *resource = ResourceFactory::GetInstance()->CreateShader(buffer);
			ResourceHandle<Program> *handle = AllocateResourceHandle(resource);
			SetToLoaded(GetDataFileName(DEFAULT_SHADER_NAME), ReinterpretCast(ResourceAnyPointer, handle));
		}
	}
}