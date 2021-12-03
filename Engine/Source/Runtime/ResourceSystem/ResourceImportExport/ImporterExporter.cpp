// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceImportExport\ImporterExporter.h>
#include <Platform\PlatformFile.h>
#include <JSON\JSONParser.h>
#include <JSON\JSONObject.h>
#include <Reflection\PropertyType.h>
#include <Platform\PlatformOS.h>
#include <Allocators\RootAllocator.h>
#include <Allocators\FrameAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Platform;
	using namespace JSON;
	using namespace Allocators;

	namespace ResourceImportExport
	{
		cwstr ImporterExporter::META_EXTENSION = L".meta";
		const int8 ImporterExporter::META_FILE_FORMAT_VERSION = 1;

		bool ImporterExporter::Invalidate(const WString& FilePath)
		{
			WString metaFilePath = GetMetaFilePath(FilePath);

			if (!PlatformFile::Exists(metaFilePath.GetValue()))
				return true;

			PlatformFile::Delete(metaFilePath.GetValue());

			return true;
		}

		WString ImporterExporter::GetMetaFilePath(const WString& FilePath)
		{
			return FilePath + ImporterExporter::META_EXTENSION;
		}

		WString ImporterExporter::GetResourceFilePath(const WString& MetaFilePath)
		{
			static const uint8 EXT_LENGTH = WString(ImporterExporter::META_EXTENSION).GetLength();

			return MetaFilePath.SubString(0, MetaFilePath.GetLength() - EXT_LENGTH);
		}

		void ImporterExporter::GetProperties(const DataStructureType& Type, TypeList& Properties)
		{
			Type.GetProperties(AccessSpecifiers::Public | AccessSpecifiers::Protected | AccessSpecifiers::Private, Properties);

			TypeList list;
			Type.GetParents(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, list);

			for (const auto parentType : list)
				GetProperties(*ReinterpretCast(DataStructureType*, parentType), Properties);
		}

		void ImporterExporter::ReadMetaFile(const WString& FilePath, TypeList& Properties, void* SettingObject)
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

		void ImporterExporter::WriteMetaFile(const WString& FilePath, TypeList& Properties, const void* SettingObject)
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
	}
}