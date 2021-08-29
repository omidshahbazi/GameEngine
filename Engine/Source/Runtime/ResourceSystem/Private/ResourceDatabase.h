// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H

#include <Containers\Strings.h>
#include <Containers\GUID.h>
#include <JSON\JSONObject.h>

namespace Engine
{
	using namespace Containers;
	using namespace JSON;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceDatabase
			{
			public:
				ResourceDatabase(const WString& LibraryPath);

				void AddCompiledResource(const WString& RelativeFilePath, const GUID& GUID);

				GUID GetGUID(const WString& RelativeFilePath) const;

				WString GetRelativeFilePath(const GUID& GUID) const;

				bool CheckDuplicate(const GUID& GUID, const WString& RelativeFilePath) const;

			private:
				void Save(void);

			private:
				WString m_FilePath;
				JSONObject m_Database;
			};
		}
	}
}
#endif