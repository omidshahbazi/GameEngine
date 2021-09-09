// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Containers\Exception.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <JSON\JSONParser.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace JSON;

	namespace ResourceSystem
	{
		namespace Private
		{
			const cwstr FileName = L"ResourceDatabase.data";

			ResourceDatabase::ResourceDatabase(const WString& LibraryPath) :
				m_FilePath(Path::Combine(LibraryPath, WString(FileName))),
				m_Allocator("Resource Database Parser", ResourceSystemAllocators::ResourceAllocator),
				m_Database(ResourceSystemAllocators::ResourceAllocator)
			{
				if (FileSystem::Exists(m_FilePath))
				{
					String data;
					THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::ReadAllText(m_FilePath, &data), "Couldn't read from resource database file");

					JSONParser::Parse(&m_Allocator, data, &m_Database);
				}
			}

			void ResourceDatabase::AddCompiledResource(const WString& RelativeFilePath, const GUID& GUID)
			{
				m_Database[RelativeFilePath.ChangeType<char8>()] = GUID.ToString();

				Save();
			}

			GUID ResourceDatabase::GetGUID(const WString& RelativeFilePath) const
			{
				const String relativeFilePath = RelativeFilePath.ChangeType<char8>();

				if (!m_Database.Contains(relativeFilePath))
					return GUID::Invalid;

				return m_Database[relativeFilePath].GetAny().GetAsString();
			}

			WString ResourceDatabase::GetRelativeFilePath(const GUID& GUID) const
			{
				const String guid = GUID.ToString();

				for (auto& item : m_Database)
				{
					if (item.GetSecond().GetAny().GetAsString() != guid)
						continue;

					return item.GetFirst().ChangeType<char16>();
				}

				return WString::Empty;
			}

			bool ResourceDatabase::CheckDuplicate(const GUID& GUID, const WString& RelativeFilePath) const
			{
				const String guid = GUID.ToString();
				const String relativeFilePath = RelativeFilePath.ChangeType<char8>();

				for (auto& item : m_Database)
				{
					if (item.GetSecond().GetAny().GetAsString() != guid)
						continue;

					if (item.GetFirst() == relativeFilePath)
						continue;

					return true;
				}

				return false;
			}

			void ResourceDatabase::Save(void)
			{
				String data = m_Database.ToString();

				THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::WriteAllText(m_FilePath, data), "Couldn't write to resource database file");
			}
		}
	}
}