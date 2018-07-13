// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformDirectory.h>

namespace Engine
{
	namespace ResourceSystem
	{
		SINGLETON_DECLARATION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			auto a = Platform::PlatformDirectory::GetFiles(L"E:/reports/NorthernFarmReporting_Services.Zorvan.NorthernFarmReportingServices.LTV_Churn_Reporter 2018-07-01 10-46-21/");

			for (; a != Platform::PlatformDirectory::DirectoryIterator(); ++a)
				const cwstr &path = a.GetPath();
		}

		ResourceManager::~ResourceManager(void)
		{
		}
	}
}