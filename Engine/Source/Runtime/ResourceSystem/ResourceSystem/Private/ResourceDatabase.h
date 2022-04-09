// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_DATABASE_H
#define RESOURCE_DATABASE_H

#include <Containers\Strings.h>
#include <Containers\GUID.h>
#include <JSON\JSONObject.h>
#include <Allocators\FrameAllocator.h>

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
				struct ResourceInfo
				{
				public:
					GUID GUID;
					WString RelativePath;
					uint64 LastWriteTime;
				};

				typedef Vector<ResourceInfo> ResourceInfoList;

			public:
				ResourceDatabase(const WString& LibraryPath);

				void UpdateCompiledResource(const ResourceInfo& Info);

				void UpdateKeepingResources(const WStringList& ExistsRelativeFilePaths, ResourceInfoList& RemovedResources);

				bool DoesResourceExists(const GUID& GUID) const;

				bool GetResourceInfo(const GUID& GUID, ResourceInfo& Info) const;
				bool GetResourceInfo(const WString& RelativeFilePath, ResourceInfo& Info) const;

			private:
				void FillResourceInfo(const JSONObject& Object, const GUID& GUID, ResourceInfo& Info) const;

				void Save(void);

			private:
				WString m_FilePath;
				FrameAllocator m_Allocator;
				JSONObject m_Database;
			};
		}
	}
}
#endif