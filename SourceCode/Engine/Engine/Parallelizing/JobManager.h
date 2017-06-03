// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Job.h>
#include <Parallelizing\Allocators.h>
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
		public:
			typedef std::function<void(void)> JobProcedure;
			typedef ThreadSafeQueue<JobProcedure> QueueType;

		private:
			JobManager(void);

		public:
			void Add(JobProcedure &&Job)
			{
				m_Jobs.Push(std::forward<JobProcedure>(Job));
			}

			static JobManager &GetInstance(void)
			{
				if (instance == nullptr)
					instance = new JobManager();

				return *instance;
			}

		private:
			static void ThreadWorker(void *Arguments);
			static void MainFiberWorker(void *Arguments);

		private:
			uint8 m_ThreadCount;
			Thread *m_Threads;
			Fiber *m_Fibers;
			QueueType m_Jobs;
			static JobManager *instance;
		};

		template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename RuturnType = Job<ResultType>> RuturnType *RunJob(Function &&Function, Parameters&&... Arguments)
		{
			RuturnType *r = (RuturnType*)AllocateMemory(&Allocators::JobAllocator, sizeof(RuturnType));

			new (r) RuturnType([=]() -> ResultType { return Function(Arguments...); });

			JobManager::GetInstance().Add([r]() { r->Do(); });

			return r;
		}
	}
}

#endif