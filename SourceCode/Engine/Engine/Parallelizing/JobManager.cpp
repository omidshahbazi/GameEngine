// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Threading\ThreadManager.h>

namespace Engine
{
	Threading::ThreadManager threadManager;

	namespace Platform
	{
		JobManager::JobManager(void)
		{
		}
	}
}