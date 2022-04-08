// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Containers\Exception.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <JSON\JSONParser.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace JSON;

	namespace ResourceSystem
	{
		namespace Private
		{
			const cwstr FileName = L"ResourceDatabase.data";
			const cstr KEY_RELATIVE_PATH = "RP";
			const cstr KEY_LAST_WRITE_TIME = "LWT";

			ResourceDatabase::ResourceDatabase(const WString& LibraryPath) :
				m_FilePath(Path::Combine(LibraryPath, WString(FileName))),
				m_Allocator("Resource Database Parser", ResourceSystemAllocators::ResourceAllocator, 16 * MegaByte),
				m_Database(ResourceSystemAllocators::ResourceAllocator)
			{
				if (FileSystem::Exists(m_FilePath))
				{
					String data;
					THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::ReadAllText(m_FilePath, &data), "Couldn't read from resource database file");

					JSONParser::Parse(&m_Allocator, data, &m_Database, true);
				}
			}

			void ResourceDatabase::UpdateCompiledResource(const ResourceInfo& Info)
			{
				JSONObject* obj = (JSONObject*)(AllocateMemory(&m_Allocator, sizeof(JSONObject)));
				Construct(obj, &m_Allocator);

				(*obj)[KEY_RELATIVE_PATH] = Info.RelativePath;
				(*obj)[KEY_LAST_WRITE_TIME] = (int64)Info.LastWriteTime;

				m_Database[Info.GUID.ToString()] = obj;

				Save();
			}

			void ResourceDatabase::RemoveResourceInfo(const WString& RelativeFilePath)
			{
				m_Database.RemoveIf([&RelativeFilePath](const auto& item)
					{
						const auto& obj = item.GetSecond().GetObject();

						return (obj[KEY_RELATIVE_PATH].GetAny().GetAsWString() == RelativeFilePath);
					});
			}

			bool ResourceDatabase::DoesResourceExists(const GUID& GUID) const
			{
				const auto guid = GUID.ToString();

				return m_Database.Contains(guid);
			}

			bool ResourceDatabase::GetResourceInfo(const GUID& GUID, ResourceInfo& Info) const
			{
				const auto guid = GUID.ToString();

				if (!m_Database.Contains(guid))
					return false;

				FillResourceInfo(m_Database[guid].GetObject(), GUID, Info);

				return true;
			}

			bool ResourceDatabase::GetResourceInfo(const WString& RelativeFilePath, ResourceInfo& Info) const
			{
				for (const auto& item : m_Database)
				{
					const auto& obj = item.GetSecond().GetObject();

					if (obj[KEY_RELATIVE_PATH].GetAny().GetAsWString() != RelativeFilePath)
						continue;

					FillResourceInfo(obj, item.GetFirst(), Info);

					return true;
				}

				return false;
			}

			void ResourceDatabase::FillResourceInfo(const JSONObject& Object, const GUID& GUID, ResourceInfo& Info) const
			{
				Info.GUID = GUID;
				Info.RelativePath = Object[KEY_RELATIVE_PATH].GetAny().GetAsWString();
				Info.LastWriteTime = Object[KEY_LAST_WRITE_TIME].GetAny().GetAsInt64();
			}

			void ResourceDatabase::Save(void)
			{
				String data = m_Database.ToString();

				THROW_IF_EXCEPTION(Categories::ResourceSystem, !FileSystem::WriteAllText(m_FilePath, data), "Couldn't write to resource database file");
			}
		}
	}
}