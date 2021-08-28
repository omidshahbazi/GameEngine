// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceImportExport\ImExporter.h>
#include <Platform\PlatformFile.h>
#include <JSON\JSONParser.h>
#include <JSON\JSONObject.h>
#include <Reflection\PropertyType.h>
#include <Platform\PlatformOS.h>
#include <Containers\GUID.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Platform;
	using namespace JSON;
	using namespace MemoryManagement::Allocator;

	namespace ResourceImportExport
	{
#define IMPLEMENT_IMPORT(SettingsType) \
	TypeList properties; \
	GetProperties(SettingsType::GetType(), properties); \
	WString metaFilePath = GetMetaFileName(FilePath); \
	if (!PlatformFile::Exists(metaFilePath.GetValue())) \
		return false; \
	ReadMetaFile(metaFilePath, properties, Settings); \
	return (Settings->LastWriteTime == PlatformFile::GetLastWriteTime(FilePath.GetValue()));

#define IMPLEMENT_EXPORT(SettingsType) \
	if (Settings->ID.GetLength() == 0) \
		Settings->ID = GUID::Create().ToString(); \
	Settings->FileFormatVersion = FILE_FORMAT_VERSION; \
	Settings->LastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue()); \
	TypeList properties; \
	GetProperties(SettingsType::GetType(), properties); \
	WString metaFilePath = GetMetaFileName(FilePath); \
	WriteMetaFile(metaFilePath, properties, Settings); \
	return true;

		cwstr ImExporter::META_EXTENSION = L".meta";
		const int8 FILE_FORMAT_VERSION = 1;

		const String& GenerateUUID(void)
		{
			str uuid;
			//PlatformOS::GenerateGUID(&uuid);

			static String result;
			result = uuid;

			return result;
		}

		void GetProperties(const DataStructureType& Type, TypeList& Properties)
		{
			Type.GetProperties(AccessSpecifiers::Public | AccessSpecifiers::Protected | AccessSpecifiers::Private, Properties);

			TypeList list;
			Type.GetParents(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, list);

			for (const auto parentType : list)
				GetProperties(*ReinterpretCast(DataStructureType*, parentType), Properties);
		}

		WString GetMetaFileName(const WString& FilePath)
		{
			return FilePath + ImExporter::META_EXTENSION;
		}

		void ReadMetaFile(const WString& FilePath, TypeList& Properties, void* SettingObject)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

			char8 str[1024];
			uint64 readCount = PlatformFile::Read(handle, str, 1024);
			str[readCount] = '\0';

			PlatformFile::Close(handle);

			FrameAllocator allocator("MetaFile parser allocator", RootAllocator::GetInstance(), MegaByte);
			JSONObject obj(&allocator);
			JSONParser::Parse(&allocator, str, &obj);

			for (auto& type : Properties)
			{
				PropertyType* prop = ReinterpretCast(PropertyType*, type);

				if (!obj.Contains(prop->GetName()))
					continue;

				const JSONData& data = obj[prop->GetName()];

				prop->SetValue(SettingObject, data.GetAny());
			}
		}

		void WriteMetaFile(const WString& FilePath, TypeList& Properties, void* SettingObject)
		{
			FrameAllocator allocator("MetaFile writer allocator", RootAllocator::GetInstance(), MegaByte);
			JSONObject obj(&allocator);

			for (auto type : Properties)
			{
				PropertyType* prop = ReinterpretCast(PropertyType*, type);

				obj[prop->GetName()] = prop->GetValue(SettingObject);
			}

			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);
			PlatformFile::Write(handle, obj.ToString().GetValue());
			PlatformFile::Close(handle);
		}

		bool ImExporter::Invalidate(const WString& FilePath)
		{
			WString metaFilePath = GetMetaFileName(FilePath);

			if (!PlatformFile::Exists(metaFilePath.GetValue()))
				return true;

			PlatformFile::Delete(metaFilePath.GetValue());

			return true;
		}

		bool ImExporter::ImportText(const WString& FilePath, TextSettings* Settings)
		{
			IMPLEMENT_IMPORT(TextSettings)
		}

		bool ImExporter::ExportText(const WString& FilePath, TextSettings* Settings)
		{
			IMPLEMENT_EXPORT(TextSettings)
		}

		bool ImExporter::ImportTexture(const WString& FilePath, TextureSettings* Settings)
		{
			IMPLEMENT_IMPORT(TextureSettings)
		}

		bool ImExporter::ExportTexture(const WString& FilePath, TextureSettings* Settings)
		{
			IMPLEMENT_EXPORT(TextureSettings)
		}

		bool ImExporter::ImportProgram(const WString& FilePath, ProgramSettings* Settings)
		{
			IMPLEMENT_IMPORT(ProgramSettings)
		}

		bool ImExporter::ExportProgram(const WString& FilePath, ProgramSettings* Settings)
		{
			IMPLEMENT_EXPORT(ProgramSettings)
		}

		bool ImExporter::ImportMesh(const WString& FilePath, MeshSettings* Settings)
		{
			IMPLEMENT_IMPORT(MeshSettings)
		}

		bool ImExporter::ExportMesh(const WString& FilePath, MeshSettings* Settings)
		{
			IMPLEMENT_EXPORT(MeshSettings)
		}

		bool ImExporter::ImportFont(const WString& FilePath, FontSettings* Settings)
		{
			IMPLEMENT_IMPORT(FontSettings)
		}

		bool ImExporter::ExportFont(const WString& FilePath, FontSettings* Settings)
		{
			IMPLEMENT_EXPORT(FontSettings)
		}
	}
}