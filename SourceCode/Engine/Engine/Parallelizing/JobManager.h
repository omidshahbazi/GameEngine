// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Job.h>
#include <Threading\Thread.h>
#include <Containers\ThreadSafeQueue.h>

#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

namespace Engine
{
	namespace Threading
	{
		class Thread;
		class Fiber;
	}

	using namespace Threading;
	using namespace Containers;

	namespace Parallelizing
	{
		class PARALLELIZING_API JobManager
		{
		private:
			JobManager(void);

		public:
			void Add(JobDescription *Description);

			static JobManager &GetInstance(void)
			{
				if (instance == nullptr)
					instance = new JobManager();

				return *instance;
			}

		private:
			static void ThreadWorker(void *Arguments);
			static void MainFiberWorker(void *Arguments);
			static void JobFiberWorker(void *Arguments);

		private:
			uint8 m_ThreadCount;
			Thread *m_Threads;
			Fiber *m_Fibers;
			ThreadSafeQueue<Job*> m_Jobs;
			static JobManager *instance;
		};

		JobDescription *CreateJobDescription(JobDescription::Procedure &&Procedure, void *Arguments);
		PARALLELIZING_API JobDescription *AddJob(JobDescription::Procedure &&Procedure);

		template <typename F, typename ...P> JobDescription *RunJob(F Function, P&&... Arguments)
		{
			return AddJob(std::forward<JobDescription::Procedure>([=]() { Function(Arguments...); }));
		}
	}
}

#endif