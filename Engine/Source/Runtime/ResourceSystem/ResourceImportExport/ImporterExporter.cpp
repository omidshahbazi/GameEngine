// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceImportExport\ImporterExporter.h>
#include <Platform\PlatformFile.h>
#include <JSON\JSONParser.h>
#include <Reflection\PropertyType.h>
#include <Platform\PlatformOS.h>
#include <Allocators\RootAllocator.h>
#include <Allocators\FrameAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Platform;
	using namespace Allocators;

	namespace ResourceImportExport
	{
		cwstr ImporterExporter::META_EXTENSION = L".meta";
		const int8 ImporterExporter::META_FILE_FORMAT_VERSION = 1;

		bool ImporterExporter::Exists(const WString& FilePath)
		{
			return PlatformFile::Exists(GetMetaFilePath(FilePath).GetValue());
		}

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

		void ImporterExporter::GetProperties(const ObjectType& Type, PropertyTypeList& Properties)
		{
			Type.GetProperties(AccessSpecifiers::Public | AccessSpecifiers::Protected | AccessSpecifiers::Private, Properties);

			ObjectTypeList list;
			Type.GetParents(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, list);

			for (const auto parentType : list)
				GetProperties(*parentType, Properties);
		}

		bool ImporterExporter::ParseMetaFile(AllocatorBase* Allocator, const WString& FilePath, JSONObject& Object)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);
			if (handle == 0)
				return false;

			char8 str[1024];
			uint64 readCount = PlatformFile::Read(handle, str, 1024);
			PlatformFile::Close(handle);

			if (readCount == 0)
				return false;
			str[readCount] = '\0';

			JSONParser::Parse(Allocator, str, &Object);

			return true;
		}

		bool ImporterExporter::ReadMetaFile(const WString& FilePath, PropertyTypeList& Properties, void* SettingObject)
		{
			FrameAllocator allocator("MetaFile parser allocator", RootAllocator::GetInstance(), MegaByte);
			JSONObject obj(&allocator);
			if (!ParseMetaFile(&allocator, FilePath, obj))
				return false;

			for (auto& prop : Properties)
			{
				if (!obj.Contains(prop->GetName()))
					continue;

				const JSONData& data = obj[prop->GetName()];

				prop->SetValue(SettingObject, data.GetAny());
			}

			return true;
		}

		bool ImporterExporter::WriteMetaFile(const WString& FilePath, PropertyTypeList& Properties, const void* SettingObject, bool Overwrite)
		{
			FrameAllocator allocator("MetaFile writer allocator", RootAllocator::GetInstance(), MegaByte);
			JSONObject obj(&allocator);

			if (!Overwrite && !ParseMetaFile(&allocator, FilePath, obj))
				return false;

			for (auto prop : Properties)
				obj[prop->GetName()] = prop->GetValue(SettingObject);

			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);
			if (handle == 0)
				return false;

			PlatformFile::Write(handle, obj.ToString().GetValue());
			PlatformFile::Close(handle);

			return true;
		}
	}
}