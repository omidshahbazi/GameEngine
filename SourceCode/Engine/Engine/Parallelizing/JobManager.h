// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <Parallelizing\Task.h>
#include <Parallelizing\Job.h>
#include <Parallelizing\Private\Allocators.h>
#include <Threading\Thread.h>
#include <Containers\ThreadSafeQueue.h>
#include <MemoryManagement\SharedMemory.h>
#include <MemoryManagement\Singleton.h>

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
		using namespace Private;

		enum class Priority
		{
			Low = 0,
			Normal,
			High
		};

		class PARALLELIZING_API JobManager
		{
			SINGLETON_DEFINITION(JobManager)

		public:
			typedef ThreadSafeQueue<Task> QueueType;
			typedef ThreadSafeQueue<Fiber*> FiberQueue;

		private:
			JobManager(void);

			~JobManager(void);

		public:
			void Add(Task::Procedure &&Procedure, Priority Priority = Priority::Normal)
			{
				m_JobsQueues[(uint8)Priority].Push(Task(Procedure));
			}

		private:
			Fiber * GetFreeFiber(void);

			static void ThreadWorker(void *Arguments);
			static void MainFiberWorker(void *Arguments);
			static void TaskFiberWorker(void *Arguments);

		private:
			uint8 m_ThreadCount;
			Thread *m_Threads;
			Fiber *m_MainFibers;
			Fiber *m_WorkerFibersPtr;
			byte *m_ThreadArguments;
			byte *m_FiberArguments;
			QueueType m_JobsQueues[(uint8)Priority::High + 1];
			FiberQueue m_WorkerFibers;
		};

		template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename ReturnType = Job<ResultType>>
		ReturnType RunJob(Function &&Function, Parameters&&... Arguments)
		{
			return RunJob(Priority::Normal, Function, std::forward<Parameters>(Arguments)...);
		}

		template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename ReturnType = Job<ResultType>>
		ReturnType RunJob(Priority Priority, Function &&Function, Parameters&&... Arguments)
		{
			JobInfo<ResultType> *info = (JobInfo<ResultType>*)AllocateMemory(&Allocators::JobAllocator, sizeof(JobInfo<ResultType>));

			//new (info) JobInfo<ResultType>(std::bind(Function, std::forward<Parameters>(Arguments)...));
			new (info) JobInfo<ResultType>([&Function, &Arguments...]()->ResultType{ return Function(std::forward<Parameters>(Arguments)...); });

			JobManager::GetInstance()->Add(std::bind(&JobInfo<ResultType>::Do, info), Priority);

			return ReturnType(info);
		}
	}
}

#endif