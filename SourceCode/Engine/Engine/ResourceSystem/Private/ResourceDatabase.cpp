// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <Containers\Exception.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <YAML\YAMLParser.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace YAML;

	namespace ResourceSystem
	{
		namespace Private
		{
			const cwstr FileName = L"ResourceDatabase.data";

			ResourceDatabase::ResourceDatabase(const WString& LibraryPath) :
				m_FilePath(Path::Combine(LibraryPath, WString(FileName)))
			{
				if (FileSystem::Exists(m_FilePath))
				{
					String data;
					THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::ReadAllText(m_FilePath, &data), "Couldn't read from resource database file");

					YAMLParser parser;
					parser.Parse(data, m_Database);
				}
			}

			void ResourceDatabase::Save(void)
			{
				String data = m_Database.ToString();

				THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::WriteAllText(m_FilePath, data), "Couldn't write to resource database file");
			}
		}
	}
}