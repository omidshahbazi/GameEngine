// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
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

		const WString META_EXTENSION(L".meta");
		const WString DATA_EXTENSION(L".data");
		const String KEY_GUID("GUID");
		const String KEY_LAST_WRITE_TIME("LastWriteTime");
		const String KEY_FILE_FORMAT_VERSION("FileFormatVersion");

		const int8 FILE_FORMAT_VERSION = 1;

		const String& GenerateUUID(void)
		{
			str uuid;
			PlatformOS::GenerateGUID(&uuid);

			static String result;
			result = uuid;

			return result;
		}

		void ReadMetaFile(const WString& Path, YAMLObject& Object)
		{
			YAMLParser parser;

			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			static char8 str[1024];
			PlatformFile::Read(handle, str, 1024);

			PlatformFile::Close(handle);

			parser.Parse(str, Object);
		}

		void WriteMetaFile(const WString& Path, YAMLObject& Object)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output);
			PlatformFile::Write(handle, Object.ToString().GetValue());
			PlatformFile::Close(handle);
		}

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
				ResourceFactory::ResourceTypes type;
				Compile(Path, type);

				ResourceAnyPointer ptr = m_LoadedResources[hash];

				switch (type)
				{
				case ResourceFactory::ResourceTypes::Text:
				{
					ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, ptr);

					Text* oldRes = **handle;

					handle->Swap(LoadInternal<Text>(Path));

					ResourceFactory::GetInstance()->DestroyText(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Texture:
				{
					ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, ptr);

					Texture* oldRes = **handle;

					handle->Swap(LoadInternal<Texture>(Path));

					ResourceFactory::GetInstance()->DestroyTexture(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Program:
				{
					ResourceHandle<Program>* handle = ReinterpretCast(ResourceHandle<Program>*, ptr);

					Program* oldRes = **handle;

					handle->Swap(LoadInternal<Program>(Path));

					ResourceFactory::GetInstance()->DestroyProgram(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Mesh:
				{
					ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, ptr);

					Mesh* oldRes = **handle;

					handle->Swap(LoadInternal<Mesh>(Path));

					ResourceFactory::GetInstance()->DestroyMesh(oldRes);
				} break;

				case ResourceFactory::ResourceTypes::Font:
				{
					ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, ptr);

					Font* oldRes = **handle;

					handle->Swap(LoadInternal<Font>(Path));

					ResourceFactory::GetInstance()->DestroyFont(oldRes);
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

			Mesh* resource = ResourceFactory::GetInstance()->CreatePrimitiveMesh(Type);

			ResourceHandle<Mesh>* handle = AllocateResourceHandle(resource);

			AddToLoaded(name, ReinterpretCast(ResourceAnyPointer, handle));

			return handle;
		}

		ProgramResource ResourceHolder::LoadProgram(const String& Name, const String& Source, String* Message)
		{
			Assert(Name.GetLength() != 0, "Name cannot be empty");
			Assert(Source.GetLength() != 0, "Source cannot be empty");

			WString name = Name.ChangeType<char16>();

			ResourceAnyPointer anyPtr = GetFromLoaded(name);

			if (anyPtr != nullptr)
				return ReinterpretCast(ResourceHandle<Program>*, anyPtr);

			ByteBuffer buffer(ReinterpretCast(byte*, ConstCast(char8*, Source.GetValue())), Source.GetLength());
			Program* resource = ResourceFactory::GetInstance()->CreateProgram(buffer, Message);

			ResourceHandle<Program>* handle = AllocateResourceHandle(resource);

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
				if (Path::GetExtension(path).ToLower() == META_EXTENSION)
					continue;

				WString finalPath = path.SubString(assetsPath.GetLength() + 1).ToLower();

				ResourceFactory::ResourceTypes type;
				Compile(finalPath, type);
			}
		}

		bool ResourceHolder::Compile(const WString& FilePath, ResourceFactory::ResourceTypes& Type)
		{
			SetAssetsWorkingPath();

			WString metaFilePath = FilePath + META_EXTENSION;
			int64 lastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue());

			WStringStream dataFilePathStream(&ResourceSystemAllocators::ResourceAllocator);
			dataFilePathStream << GetLibraryPath() << "/" << GetDataFileName(FilePath) << '\0';
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

		bool ResourceHolder::CompileFile(const WString& FilePath, const WString& DataFilePath, ResourceFactory::ResourceTypes& Type)
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

		void ResourceHolder::SetAssetsWorkingPath(void)
		{
			SetWorkingPath(GetAssetsPath());
		}

		void ResourceHolder::SetLibraryWorkingPath(void)
		{
			SetWorkingPath(GetLibraryPath());
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

		WString ResourceHolder::GetDataFileName(const WString& FilePath)
		{
			WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
			uint32 hash = GetHash(FilePath);
			stream << hash << DATA_EXTENSION << '\0';
			return stream.GetBuffer();
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

		void ResourceHolder::CheckDirectories(void)
		{
			WString dir = GetAssetsPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir = GetLibraryPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());
		}
	}
}