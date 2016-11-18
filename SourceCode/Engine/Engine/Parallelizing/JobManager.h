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
	template <typename ...Parameters> struct __Job_##Name \
	{ \
		template <typename ...Parameters> friend static JobDescription *Execute(Class *Instance, Parameters... Params); \
		friend static void Worker(void *Arguments); \
	public: \
		template <typename ...Parameters> static JobDescription *Execute(Class *Instance, Parameters... Params) \
		{ \
			__Job_##Name<Parameters...> *arguments = new __Job_##Name<Parameters...>; \
			arguments->Instance = Instance; \
			arguments->Arguments = std::tuple<Parameters...>(Params...); \
			return AddJob(__Job_##Name<Parameters...>::Worker, arguments); \
		} \
	private: \
		static void Worker(void *Arguments) \
		{ \
			__Job_##Name *arguments = reinterpret_cast<__Job_##Name*>(Arguments); \
			arguments->Instance->Name(std::get<Parameters>(arguments->Arguments)...); \
		} \
	private: \
		Class *Instance; \
		std::tuple<Parameters...> Arguments; \
	};

#define DECLARE_JOB(Class, Name) \
	DECLARE_JOB_STRUCTURE(Class, Name) \
	void Name(void)

#define DECLARE_JOB_1_ARGUMENT(Class, Name, Argument) \
	DECLARE_JOB_STRUCTURE(Class, Name) \
	void Name(Argument)

#define RUN_JOB(Name, ...) __Job_##Name<>::Execute(this, __VA_ARGS__)
	}
}

#endif