// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\MainFiberWorkerArguments.h>
#include <Parallelizing\Private\ThreadWorkerArguments.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		using namespace Private;

		SINGLETON_DEFINITION(JobManager)

			const uint8 WORKER_FIBERS_COUNT = 128;

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = ParallelizingAllocators::ThreadAllocator_AllocateArray<Thread>(m_ThreadCount);
			m_MainFibers = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(m_ThreadCount);
			m_WorkerFibersPtr = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(WORKER_FIBERS_COUNT);

			ThreadWorkerArguments* threadArguments = ParallelizingAllocators::ThreadWorkerArgumentsAllocator_AllocateArray<ThreadWorkerArguments>(m_ThreadCount);
			MainFiberWorkerArguments* fiberArguments = ParallelizingAllocators::MainFiberWorkerArgumentAllocator_AllocateArray<MainFiberWorkerArguments>(m_ThreadCount);

			m_ThreadArguments = ReinterpretCast(byte*, threadArguments);
			m_FiberArguments = ReinterpretCast(byte*, fiberArguments);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread& thread = m_Threads[i];
				new (&thread) Thread;

				Fiber& fiber = m_MainFibers[i];
				new (&fiber) Fiber;

				ThreadWorkerArguments& thdArg = threadArguments[i];
				thdArg.Thread = &thread;
				thdArg.Fiber = &fiber;

				MainFiberWorkerArguments& fbrArg = fiberArguments[i];
				fbrArg.Thread = threadArguments->Thread;
				fbrArg.MainFiber = &fiber;
				fbrArg.JobsQueues = m_JobsQueues;
				fbrArg.WorkerFiberQueue = &m_WorkerFibers;

				fiber.Initialize((PlatformFiber::Procedure) & JobManager::MainFiberWorker, sizeof(void*) * 4, &fbrArg);

				thread.Initialize((PlatformThread::Procedure) & JobManager::ThreadWorker, sizeof(void*) * 2, &thdArg);
				thread.SetCoreAffinity(i);
			}

			for (uint8 i = 0; i < WORKER_FIBERS_COUNT; ++i)
			{
				Fiber* fiber = &m_WorkerFibersPtr[i];

				new (fiber) Fiber;

				m_WorkerFibers.Push(fiber);
			}
		}

		JobManager::~JobManager(void)
		{
			ParallelizingAllocators::ThreadWorkerArgumentsAllocator_Deallocate(m_ThreadArguments);
			ParallelizingAllocators::MainFiberWorkerArgumentAllocator_Deallocate(m_FiberArguments);
			ParallelizingAllocators::ThreadAllocator_Deallocate(m_Threads);
			ParallelizingAllocators::FiberAllocator_Deallocate(m_MainFibers);
			ParallelizingAllocators::FiberAllocator_Deallocate(m_WorkerFibersPtr);
		}

		Fiber* JobManager::GetFreeFiber(void)
		{
			return nullptr;
		}

		void JobManager::ThreadWorker(void* Arguments)
		{
			ThreadWorkerArguments* arguments = ReinterpretCast(ThreadWorkerArguments*, Arguments);

			Fiber* fiber = arguments->Fiber;

			fiber->ConvertThreadToFiber(nullptr);
			fiber->Switch();
		}

		void JobManager::MainFiberWorker(void* Arguments)
		{
			MainFiberWorkerArguments* arguments = ReinterpretCast(MainFiberWorkerArguments*, Arguments);

			Task task = nullptr;

			while (arguments->JobsQueues != nullptr)
			{
				uint8 priority = (uint8)Priority::High;

				while (true)
				{
					if (arguments->JobsQueues == nullptr)
						return;

					if (arguments->JobsQueues[priority].Pop(&task))
						break;

					if (priority-- == (uint8)Priority::Low)
						priority = (uint8)Priority::High;

					arguments->Thread->Sleep(1000);
				}

				Fiber* fiber = nullptr;
				if (!arguments->WorkerFiberQueue->Pop(&fiber))
					continue;

				TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
				fiberArguments->MainFiber = arguments->MainFiber;
				fiberArguments->Task = &task;
				fiberArguments->CurrentFiber = fiber;
				fiberArguments->WorkerFiberQueue = arguments->WorkerFiberQueue;

				fiber->Initialize(TaskFiberWorker, 32, fiberArguments);

				fiber->Switch();
			}
		}

		void JobManager::TaskFiberWorker(void* Arguments)
		{
			TaskFiberWorkerArguments* arguments = ReinterpretCast(TaskFiberWorkerArguments*, Arguments);

			arguments->Task->Do();

			Fiber* fiber = arguments->MainFiber;

			arguments->WorkerFiberQueue->Push(arguments->CurrentFiber);

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(arguments);

			fiber->Switch();
		}
	}
}