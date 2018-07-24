// Copyright 2016-2017 ?????????????. All Rights Reserved.
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

		SINGLETON_DECLARATION(JobManager)

		const uint8 WORKER_FIBERS_COUNT = 128;

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = reinterpret_cast<Thread*>(AllocateMemory(&ParallelizingAllocators::ThreadAllocator, m_ThreadCount));
			m_MainFibers = reinterpret_cast<Fiber*>(AllocateMemory(&ParallelizingAllocators::FiberAllocator, m_ThreadCount));
			m_WorkerFibersPtr = reinterpret_cast<Fiber*>(AllocateMemory(&ParallelizingAllocators::FiberAllocator, WORKER_FIBERS_COUNT));

			ThreadWorkerArguments *threadArguments = reinterpret_cast<ThreadWorkerArguments*>(AllocateMemory(&ParallelizingAllocators::ThreadWorkerArgumentsAllocator, m_ThreadCount));
			MainFiberWorkerArguments *fiberArguments = reinterpret_cast<MainFiberWorkerArguments*>(AllocateMemory(&ParallelizingAllocators::MainFiberWorkerArgumentAllocator, m_ThreadCount));

			m_ThreadArguments = reinterpret_cast<byte*>(threadArguments);
			m_FiberArguments = reinterpret_cast<byte*>(fiberArguments);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];
				new (&thread) Thread;

				Fiber &fiber = m_MainFibers[i];
				new (&fiber) Fiber;

				ThreadWorkerArguments &thdArg = threadArguments[i];
				thdArg.Thread = &thread;
				thdArg.Fiber = &fiber;

				MainFiberWorkerArguments &fbrArg = fiberArguments[i];
				fbrArg.Thread = threadArguments->Thread;
				fbrArg.MainFiber = &fiber;
				fbrArg.JobsQueues = m_JobsQueues;
				fbrArg.WorkerFiberQueue = &m_WorkerFibers;

				fiber.Initialize((PlatformFiber::Procedure)&JobManager::MainFiberWorker, sizeof(void*) * 4, &fbrArg);

				thread.Initialize((PlatformThread::Procedure)&JobManager::ThreadWorker, sizeof(void*) * 2, &thdArg);
				thread.SetCoreAffinity(i);
			}

			for (uint8 i = 0; i < WORKER_FIBERS_COUNT; ++i)
			{
				Fiber *fiber = &m_WorkerFibersPtr[i];

				new (fiber) Fiber;

				m_WorkerFibers.Push(fiber);
			}
		}

		JobManager::~JobManager(void)
		{
			DeallocateMemory(&ParallelizingAllocators::ThreadWorkerArgumentsAllocator, m_ThreadArguments);
			DeallocateMemory(&ParallelizingAllocators::MainFiberWorkerArgumentAllocator, m_FiberArguments);
			DeallocateMemory(&ParallelizingAllocators::ThreadAllocator, m_Threads);
			DeallocateMemory(&ParallelizingAllocators::FiberAllocator, m_MainFibers);
			DeallocateMemory(&ParallelizingAllocators::FiberAllocator, m_WorkerFibersPtr);
		}

		Fiber * JobManager::GetFreeFiber(void)
		{
			return nullptr;
		}

		void JobManager::ThreadWorker(void *Arguments)
		{
			ThreadWorkerArguments *arguments = reinterpret_cast<ThreadWorkerArguments*>(Arguments);

			Fiber *fiber = arguments->Fiber;

			fiber->ConvertThreadToFiber(nullptr);
			fiber->Switch();
		}

		void JobManager::MainFiberWorker(void *Arguments)
		{
			MainFiberWorkerArguments *arguments = reinterpret_cast<MainFiberWorkerArguments*>(Arguments);

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

				Fiber *fiber = nullptr;
				if (!arguments->WorkerFiberQueue->Pop(&fiber))
					continue;

				TaskFiberWorkerArguments *fiberArguments = reinterpret_cast<TaskFiberWorkerArguments*>(AllocateMemory(&ParallelizingAllocators::TaskFiberWorkerArgumentAllocator, 1));
				fiberArguments->MainFiber = arguments->MainFiber;
				fiberArguments->Task = &task;
				fiberArguments->CurrentFiber = fiber;
				fiberArguments->WorkerFiberQueue = arguments->WorkerFiberQueue;

				fiber->Initialize(TaskFiberWorker, 32, fiberArguments);

				fiber->Switch();
			}
		}

		void JobManager::TaskFiberWorker(void *Arguments)
		{
			TaskFiberWorkerArguments *arguments = reinterpret_cast<TaskFiberWorkerArguments*>(Arguments);

			arguments->Task->Do();

			Fiber *fiber = arguments->MainFiber;

			arguments->WorkerFiberQueue->Push(arguments->CurrentFiber);

			DeallocateMemory(&ParallelizingAllocators::TaskFiberWorkerArgumentAllocator, arguments);

			fiber->Switch();
		}
	}
}