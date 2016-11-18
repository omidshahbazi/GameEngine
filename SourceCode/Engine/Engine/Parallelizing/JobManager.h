// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Job.h>
#include <Threading\Thread.h>
#include <Common\ThreadSafeQueue.h>

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

	namespace Parallelizing
	{
		class JobManager
		{
		public:
			JobManager(void);

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

		JobDescription *CreateJobDescription(JobDescription::Procedure Procedure, void *Arguments);
		JobDescription *AddJob(JobDescription::Procedure Procedure, void *Arguments);

#ifndef DECLARE_JOB
#define DECLARE_JOB(Class, Name) \
	template <typename ...Parameters> struct __Job_##Name \
	{ \
	public: \
		template <typename ...Parameters> static JobDescription *Execute(Class *Instance, Parameters... Params) \
		{ \
			__Job_##Name<Parameters...> *arguments = new __Job_##Name<Parameters...>; \
			arguments->Instance = Instance; \
			arguments->Arguments = std::tuple<Parameters...>(Params...); \
			return AddJob(__Job_worker<Parameters...>::Worker, arguments); \
		} \
	private: \
		static void Worker(void *Arguments) \
		{ \
			__Job_##Name *arguments = reinterpret_cast<__Job_##Name*>(Arguments); \
			arguments->Instance->worker(std::get<Parameters>(arguments->Arguments)...); \
		} \
	private: \
		Class *Instance; \
		std::tuple<Parameters...> Arguments; \
	}; \
	void Name(void)
#endif

#ifndef RUN_JOB
#define RUN_JOB(Name) __Job_##Name<>::Execute(this)
#endif
	}
}

#endif