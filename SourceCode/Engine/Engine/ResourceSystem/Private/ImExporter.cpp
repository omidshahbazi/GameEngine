// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ImExporter.h>
#include <ResourceSystem\Private\Utilities.h>
#include <Platform\PlatformFile.h>
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>
#include <Reflection\PropertyType.h>
#include <Platform\PlatformOS.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Platform;
	using namespace Utility::YAML;

	namespace ResourceSystem
	{
		namespace Private
		{
#define IMPLEMENT_IMPORT(SettingsType) \
	TypeList properties; \
	GetProperties(SettingsType::GetType(), properties); \
	WString metaFilePath = GetMetaFileName(FilePath); \
	if (!PlatformFile::Exists(metaFilePath.GetValue())) \
		return true; \
	ReadMetaFile(metaFilePath, properties, Settings); \
	return (Settings->LastWriteTime != PlatformFile::GetLastWriteTime(FilePath.GetValue()));

#define IMPLEMENT_EXPORT(SettingsType) \
	if (Settings->ID.GetLength() == 0) \
		Settings->ID = GenerateUUID(); \
	Settings->FileFormatVersion = FILE_FORMAT_VERSION; \
	Settings->LastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue()); \
	TypeList properties; \
	GetProperties(SettingsType::GetType(), properties); \
	WString metaFilePath = GetMetaFileName(FilePath); \
	WriteMetaFile(metaFilePath, properties, Settings); \
	return true;

			const int8 FILE_FORMAT_VERSION = 1;

			const String& GenerateUUID(void)
			{
				str uuid;
				PlatformOS::GenerateGUID(&uuid);

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
				return FilePath + Utilities::META_EXTENSION;
			}

			void ReadMetaFile(const WString& FilePath, TypeList& Properties, void* SettingObject)
			{
				auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

				static char8 str[1024];
				uint64 readCount = PlatformFile::Read(handle, str, 1024);
				str[readCount] = '\0';

				PlatformFile::Close(handle);

				YAMLObject obj;
				static YAMLParser parser;
				parser.Parse(str, obj);

				for (auto& type : Properties)
				{
					PropertyType* prop = ReinterpretCast(PropertyType*, type);

					if (!obj.Contains(prop->GetName()))
						continue;

					const YAMLData& data = obj[prop->GetName()];

					prop->SetValue(SettingObject, data.GetAsAny());
				}
			}

			void WriteMetaFile(const WString& FilePath, TypeList& Properties, void* SettingObject)
			{
				YAMLObject obj;

				for (auto type : Properties)
				{
					PropertyType* prop = ReinterpretCast(PropertyType*, type);

					obj[prop->GetName()] = prop->GetValue(SettingObject);
				}

				auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);
				PlatformFile::Write(handle, obj.ToString().GetValue());
				PlatformFile::Close(handle);
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

			bool ImExporter::ImportShader(const WString& FilePath, ShaderSettings* Settings)
			{
				IMPLEMENT_IMPORT(ShaderSettings)
			}

			bool ImExporter::ExportShader(const WString& FilePath, ShaderSettings* Settings)
			{
				IMPLEMENT_EXPORT(ShaderSettings)
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
}