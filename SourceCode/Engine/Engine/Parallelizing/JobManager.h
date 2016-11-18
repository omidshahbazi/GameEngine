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

		JobDescription *CreateJobDescription(JobDescription::Procedure Procedure, void *Arguments);
		JobDescription *AddJob(JobDescription::Procedure Procedure, void *Arguments);

#define DECLARE_JOB_STRUCTURE(Class, Name) \
	template <typename ...Parameters> struct __Job_##Name##_Arguments \
	{ \
		Class *Instance; \
		std::tuple<Parameters...> Arguments; \
	}; \
	template <typename ...Parameters> static JobDescription *__Execute_Job_##Name(Class *Instance, Parameters... Params) \
	{ \
		__Job_##Name##_Arguments<Parameters...> *arguments = new __Job_##Name##_Arguments<Parameters...>; \
		arguments->Instance = Instance; \
		arguments->Arguments = std::tuple<Parameters...>(Params...); \
		return AddJob(__Job_##Name##_Worker<Parameters...>, arguments); \
	} \
	template <typename ...Parameters> static void __Job_##Name##_Worker(void *Arguments) \
	{ \
		__Job_##Name##_Arguments<Parameters...> *arguments = reinterpret_cast<__Job_##Name##_Arguments<Parameters...>*>(Arguments); \
		arguments->Instance->Name(std::get<Parameters>(arguments->Arguments)...); \
	}

#define DECLARE_JOB(Class, Name) \
	DECLARE_JOB_STRUCTURE(Class, Name) \
	void Name(void)

#define DECLARE_JOB_1_ARGUMENT(Class, Name, Argument) \
	DECLARE_JOB_STRUCTURE(Class, Name) \
	void Name(Argument)

#define RUN_JOB(Name, ...) __Execute_Job_##Name<...>(this, __VA_ARGS__)
	}
}

#endif