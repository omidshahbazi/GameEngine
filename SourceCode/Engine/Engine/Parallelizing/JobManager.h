// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Job.h>
#include <Parallelizing\Allocators.h>
#include <Threading\Thread.h>
#include <Containers\ThreadSafeQueue.h>
#include <MemoryManagement\SharedMemory.h>

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
	using namespace MemoryManagement;

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
			void Add(JobProcedure Job)
			{
				m_Jobs.Push(Job);
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

		template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename ReturnType = Job<ResultType>> ReturnType RunJob(Function &&Function, Parameters&&... Arguments)
		{
			JobInfo<ResultType> *info = (JobInfo<ResultType>*)AllocateMemory(&Allocators::JobAllocator, sizeof(JobInfo<ResultType>));

			new (info) JobInfo<ResultType>(std::bind(Function, Arguments...));

			JobManager::GetInstance().Add(std::bind(&JobInfo<ResultType>::Do, info));

			return ReturnType(info);
		}

		//template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename RuturnType = Job<ResultType>> SharedMemory<RuturnType> RunJob(Function &&Function, Parameters&&... Arguments)
		//{
		//	SharedMemory<RuturnType> r = NewSharedMemory<RuturnType>(Allocators::JobAllocator, [Function, Arguments...]()->ResultType { return Function(Arguments...); });

		//	JobManager::GetInstance().Add(&*r);

		//	return r;
		//}
	}
}

#endif