// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\Task.h>
#include <Parallelizing\Job.h>
#include <Parallelizing\Private\Allocators.h>
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
		using namespace Private;

		enum class Priority
		{
			Low = 0,
			Normal,
			High
		};

		class PARALLELIZING_API JobManager
		{
		public:
			typedef ThreadSafeQueue<Task> QueueType;

		private:
			JobManager(void);

		public:
			void Add(Task::Procedure &&Procedure, Priority Priority = Priority::Normal)
			{
				m_JobsQueues[(uint8)Priority].Push(Task(Procedure));
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
			static void TaskFiberWorker(void *Arguments);

		private:
			uint8 m_ThreadCount;
			Thread *m_Threads;
			Fiber *m_Fibers;
			QueueType m_JobsQueues[(uint8)Priority::High + 1];
			static JobManager *instance;
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

			JobManager::GetInstance().Add(std::bind(&JobInfo<ResultType>::Do, info), Priority);

			return ReturnType(info);
		}
	}
}

#endif