// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H

#include <Containers\Strings.h>
#include <YAML\YAMLObject.h>

namespace Engine
{
	using namespace Containers;
	using namespace YAML;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceDatabase
			{
				ResourceDatabase(const WString& LibraryPath);

				void Save(void);

			private:
				WString m_FilePath;
				YAMLObject m_Database;
			};
		}
	}
}
#endif