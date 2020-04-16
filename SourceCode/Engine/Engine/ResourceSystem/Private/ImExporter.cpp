// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ImExporter.h>
#include <Platform\PlatformFile.h>
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Platform;
	using namespace Utility::YAML;

	namespace ResourceSystem
	{
		namespace Private
		{
			void GetProperties(const DataStructureType& Type, TypeList& Properties)
			{
				Type.GetProperties(AccessSpecifiers::Public | AccessSpecifiers::Protected | AccessSpecifiers::Private, Properties);

				TypeList list;
				Type.GetParents(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, list);

				for each (const auto parentType in list)
					GetProperties(*ReinterpretCast(DataStructureType*, parentType), Properties);
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

			bool ImExporter::ImportText(const WString& Path, TextSettings* Settings)
			{
				TypeList properties;
				GetProperties(ImExporter::TextSettings::GetType(), properties);

				return false;
			}
		}
	}
}