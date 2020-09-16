// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <Parallelizing\Task.h>
#include <Parallelizing\Job.h>
#include <Parallelizing\Private\ParallelizingAllocators.h>
#include <Threading\Thread.h>
#include <Containers\ThreadSafeQueue.h>
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
		namespace Private
		{
			struct MainFiberWorkerArguments;
			struct ThreadWorkerArguments;
		}

		using namespace Private;

		enum class Priority
		{
			Low = 0,
			Normal,
			High
		};

		class PARALLELIZING_API JobManager
		{
			SINGLETON_DECLARATION(JobManager)

		public:
			typedef ThreadSafeQueue<Task> QueueType;
			typedef ThreadSafeQueue<Fiber*> FiberQueue;

		private:
			JobManager(void);
			~JobManager(void);

		public:
			void Add(Task::Procedure&& Procedure, Priority Priority = Priority::Normal)
			{
				m_JobsQueues[(uint8)Priority].Push(Task(Procedure));
			}

		private:
			Fiber* GetFreeFiber(void);

			static void ThreadWorker(void* Arguments);
			static void MainFiberWorker(void* Arguments);
			static void TaskFiberWorker(void* Arguments);

		private:
			uint8 m_ThreadCount;
			Thread* m_Threads;
			Fiber* m_MainFibers;
			Fiber* m_WorkerFibersPtr;
			ThreadWorkerArguments* m_ThreadArguments;
			MainFiberWorkerArguments* m_FiberArguments;
			QueueType m_JobsQueues[(uint8)Priority::High + 1];
			FiberQueue m_WorkerFibers;
		};

		template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type, typename ReturnType = Job<ResultType>>
		ReturnType RunJob(FunctionType Function, ParametersType&& ... Arguments)
		{
			return RunJob(Priority::Normal, Function, std::forward<ParametersType>(Arguments)...);
		}

		template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type, typename ReturnType = Job<ResultType>>
		ReturnType RunJob(Priority Priority, FunctionType&& Function, ParametersType&& ... Arguments)
		{
			JobInfo<ResultType>* info = ParallelizingAllocators::JobAllocator_Allocate<JobInfo<ResultType>>();

			//new (info) JobInfo<ResultType>(std::bind(Function, std::forward<Parameters>(Arguments)...));
			Construct(info, [&Function, &Arguments...]()->ResultType{ return Function(std::forward<ParametersType>(Arguments)...); });

			JobManager::GetInstance()->Add(std::bind(&JobInfo<ResultType>::Do, info), Priority);

			return ReturnType(info);
		}
	}
}

#endif