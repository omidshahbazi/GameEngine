// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		using namespace Private;

		SINGLETON_DEFINITION(JobManager);

		const uint8 WORKER_FIBERS_COUNT = 128;

		JobManager::JobManager(void)
		{
			ParallelizingAllocators::Create();

			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = ParallelizingAllocators::ThreadAllocator_AllocateArray<Thread>(m_ThreadCount);
			m_MainFibers = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(m_ThreadCount);
			m_WorkerFibersPtr = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(WORKER_FIBERS_COUNT);

			m_ThreadArguments = ParallelizingAllocators::ThreadWorkerArgumentsAllocator_AllocateArray<ThreadWorkerArguments>(m_ThreadCount);
			m_FiberArguments = ParallelizingAllocators::MainFiberWorkerArgumentAllocator_AllocateArray<MainFiberWorkerArguments>(m_ThreadCount);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread& thread = m_Threads[i];
				Construct(&thread);

				Fiber& fiber = m_MainFibers[i];
				Construct(&fiber);

				ThreadWorkerArguments& thdArg = m_ThreadArguments[i];
				thdArg.Thread = &thread;
				thdArg.Fiber = &fiber;

				MainFiberWorkerArguments& fbrArg = m_FiberArguments[i];
				fbrArg.Thread = m_ThreadArguments->Thread;
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

				Construct(fiber);

				m_WorkerFibers.Push(fiber);
			}
		}

		JobManager::~JobManager(void)
		{
			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_ThreadArguments[i]);
			ParallelizingAllocators::ThreadWorkerArgumentsAllocator_Deallocate(m_ThreadArguments);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_FiberArguments[i]);
			ParallelizingAllocators::MainFiberWorkerArgumentAllocator_Deallocate(m_FiberArguments);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_Threads[i]);
			ParallelizingAllocators::ThreadAllocator_Deallocate(m_Threads);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_MainFibers[i]);
			ParallelizingAllocators::FiberAllocator_Deallocate(m_MainFibers);

			for (uint8 i = 0; i < WORKER_FIBERS_COUNT; ++i)
				Destruct(&m_WorkerFibersPtr[i]);
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