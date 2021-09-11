// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <Parallelizing\Job.h>
#include <Parallelizing\Private\ParallelizingAllocators.h>
#include <Threading\Thread.h>
#include <Containers\Queue.h>
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
			//struct ThreadWorkerArguments;
			//struct MainFiberWorkerArguments;
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

		public:
			typedef std::function<void(void)> ProcedureType;

		private:
			struct WaitingTaskInfo
			{
			public:
				Fiber* Fiber;
				JobInfoHandle* WaitingForHandle;
			};

			template<typename T>
			class ThreadSafeQueue : public Queue<T>
			{
			public:
				ThreadSafeQueue(void)
				{
				}

				ThreadSafeQueue(AllocatorBase* Allocator) :
					Queue<T>(Allocator)
				{
				}

				INLINE void Enqueue(T& Value)
				{
					ScopeGaurd gaurd(m_Lock);

					Queue<T>::Enqueue(Value);
				}

				INLINE bool Dequeue(T* Value)
				{
					ScopeGaurd gaurd(m_Lock);

					if (Queue<T>::GetSize() == 0)
						return false;

					Queue<T>::Dequeue(Value);

					return true;
				}

			private:
				SpinLock m_Lock;
			};

			typedef ThreadSafeQueue<JobInfoHandle*> JobQueue;
			typedef ThreadSafeQueue<Fiber*> FiberQueue;
			typedef Vector<WaitingTaskInfo> WaitingTaskInfoList;

		private:
			JobManager(void);
			~JobManager(void);

		public:
#ifdef _HAS_CXX17
			template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::invoke_result<FunctionType, ParameterTypes...>::type>
#elif _HAS_CXX14
			template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::result_of<FunctionType>::type>
#endif
			Job<ResultType> AddJob(FunctionType Function, ParameterTypes... Arguments)
			{
				return AddJob(Priority::Normal, Function, Arguments...);
			}

#ifdef _HAS_CXX17
			template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::invoke_result<FunctionType, ParameterTypes...>::type>
#elif _HAS_CXX14
			template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::result_of<FunctionType>::type>
#endif
			Job<ResultType> AddJob(Priority Priority, FunctionType Function, ParameterTypes... Arguments)
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

			void ThreadWorker(uint32 ArgumentIndex);
			void MainFiberWorker(uint32 ArgumentIndex);
			bool RunHandle(Fiber& Parent, JobInfoHandle* Handle);
			void TaskFiberWorker(TaskFiberWorkerArguments& Arguments);

		private:
			uint8 m_ThreadCount;
			Thread* m_Threads;
			Fiber* m_MainFibers;
			JobQueue m_JobQueues[(uint8)Priority::High + 1];
			FiberQueue m_WorkerFibers;
			WaitingTaskInfoList m_WaitingTaskInfos;
		};

#ifdef _HAS_CXX17
		template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::invoke_result<FunctionType, ParameterTypes...>::type>
#elif _HAS_CXX14
		template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::result_of<FunctionType>::type>
#endif
		Job<ResultType> RunJob(FunctionType Function, ParameterTypes... Arguments)
		{
			return JobManager::GetInstance()->AddJob(Priority::Normal, Function, Arguments...);
		}

#ifdef _HAS_CXX17
		template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::invoke_result<FunctionType, ParameterTypes...>::type>
#elif _HAS_CXX14
		template<typename FunctionType, typename ...ParameterTypes, typename ResultType = std::result_of<FunctionType>::type>
#endif
		Job<ResultType> RunJob(Priority Priority, FunctionType Function, ParameterTypes... Arguments)
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