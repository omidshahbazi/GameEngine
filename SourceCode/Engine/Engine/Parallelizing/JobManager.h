// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Job.h>
#include <Threading\Thread.h>

#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

namespace Engine
{
	namespace Threading
	{
		class Thread;
	}

	using namespace Threading;

	namespace Parallelizing
	{
		class JobManager
		{
		public:
			JobManager(void);

			Job *Add(Job::Procedure *Procedure);

		private:
			static void ThreadWorker(void *Arguments);

		private:
			uint8 m_ThreadCount;
			Thread *m_Threads;
		};
	}
}

#endif