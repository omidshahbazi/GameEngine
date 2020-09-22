// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

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
			struct ThreadWorkerArguments;
			struct MainFiberWorkerArguments;
			struct TaskFiberWorkerArguments;
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
			SINGLETON_DECLARATION(JobManager);

			friend class MainFiberWorkerArguments;
			friend class TaskFiberWorkerArguments;

		public:
			typedef std::function<void(void)> ProcedureType;

		private:
			struct WaitingTaskInfo
			{
			public:
				Fiber* Fiber;
				JobInfoHandle* WaitingForHandle;
			};

			typedef ThreadSafeQueue<JobInfoHandle*> JobQueue;
			typedef ThreadSafeQueue<Fiber*> FiberQueue;
			typedef Vector<WaitingTaskInfo> WaitingTaskInfoList;

		private:
			JobManager(void);
			~JobManager(void);

		public:
			template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type>
			Job<ResultType> AddJob(FunctionType Function, ParametersType... Arguments)
			{
				return AddJob(Priority::Normal, Function, Arguments...);
			}

			template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type>
			Job<ResultType> AddJob(Priority Priority, FunctionType Function, ParametersType... Arguments)
			{
				JobInfo<ResultType>* info = ParallelizingAllocators::JobAllocator_Allocate<JobInfo<ResultType>>();

				auto callback = [Function, Arguments...]()->ResultType
				{
					return Function(Arguments...);
				};

				ConstructMacro(JobInfo<ResultType>, info, callback);

				AddJob(info, Priority);

				return Job<ResultType>(info);
			}

			template<typename ReturnType>
			void WaitFor(const Job<ReturnType>& Job)
			{
				WaitFor(Job.m_Info);
			}

		private:
			void AddJob(JobInfoHandle* Handle, Priority Priority = Priority::Normal);

			void WaitFor(JobInfoHandle* Handle);


			static void ThreadWorker(void* Arguments);
			static void MainFiberWorker(void* Arguments);
			static bool RunHandle(MainFiberWorkerArguments* Arguments, JobInfoHandle* Handle);
			static void TaskFiberWorker(void* Arguments);

		private:
			uint8 m_ThreadCount;
			Thread* m_Threads;
			Fiber* m_MainFibers;
			ThreadWorkerArguments* m_ThreadArguments;
			MainFiberWorkerArguments* m_FiberArguments;
			JobQueue m_JobQueues[(uint8)Priority::High + 1];
			FiberQueue m_WorkerFibers;
			WaitingTaskInfoList m_WaitingTaskInfos;
		};

		template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type>
		Job<ResultType> RunJob(FunctionType Function, ParametersType ... Arguments)
		{
			return JobManager::GetInstance()->AddJob(Priority::Normal, Function, Arguments...);
		}

		template<typename FunctionType, typename ...ParametersType, typename ResultType = std::result_of<FunctionType(ParametersType...)>::type>
		Job<ResultType> RunJob(Priority Priority, FunctionType Function, ParametersType... Arguments)
		{
			return JobManager::GetInstance()->AddJob(Priority, Function, Arguments...);
		}

		template<typename ReturnType>
		void WaitFor(const Job<ReturnType>& Job)
		{
			JobManager::GetInstance()->WaitFor(Job);
		}

		template<typename ReturnType>
		void WaitFor(Job<ReturnType>* Jobs, uint16 Count)
		{
			JobManager* manager = JobManager::GetInstance();

			for (uint16 i = 0; i < Count; ++i)
				manager->WaitFor(Jobs[i]);
		}
	}
}

#endif