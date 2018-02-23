// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		using namespace Private;

		struct ThreadWorkerArguments
		{
		public:
			Thread *Thread;
			Fiber *Fiber;
		};

		struct MainFiberWorkerArguments
		{
		public:
			Thread *Thread;
			Fiber *Fiber;
			JobManager::QueueType *JobsQueues;
		};

		struct TaskFiberWorkerArguments
		{
		public:
			Fiber *MainFiber;
			Task *Task;
		};

		FixedSizeAllocator ThreadAllocator("Thread Allocator", &Allocators::JobSystemAllocator, sizeof(Thread), 32);
		FixedSizeAllocator FiberAllocator("Fiber Allocator", &Allocators::JobSystemAllocator, sizeof(Fiber), 1000);
		FixedSizeAllocator ThreadWorkerArgumentsAllocator("Thread Worker Argument Allocator", &Allocators::JobSystemAllocator, sizeof(ThreadWorkerArguments), 32);
		FixedSizeAllocator MainFiberWorkerArgumentAllocator("Main Fiber Worker Arguments Allocator", &Allocators::JobSystemAllocator, sizeof(MainFiberWorkerArguments), 32);
		FixedSizeAllocator TaskFiberWorkerArgumentAllocator("Task Fiber Worker Arguments Allocator", &Allocators::JobSystemAllocator, sizeof(TaskFiberWorkerArguments), 1000);

		JobManager *JobManager::instance = nullptr;

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = reinterpret_cast<Thread*>(AllocateMemory(&ThreadAllocator, m_ThreadCount));
			m_Fibers = reinterpret_cast<Fiber*>(AllocateMemory(&FiberAllocator, m_ThreadCount));

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];
				new (&thread) Thread;

				Fiber &fiber = m_Fibers[i];
				new (&fiber) Fiber;

				ThreadWorkerArguments *threadArguments = reinterpret_cast<ThreadWorkerArguments*>(AllocateMemory(&ThreadWorkerArgumentsAllocator, 1));
				threadArguments->Thread = &thread;
				threadArguments->Fiber = &fiber;

				MainFiberWorkerArguments *fiberArguments = reinterpret_cast<MainFiberWorkerArguments*>(AllocateMemory(&MainFiberWorkerArgumentAllocator, 1));
				fiberArguments->Thread = threadArguments->Thread;
				fiberArguments->Fiber = &fiber;
				fiberArguments->JobsQueues = m_JobsQueues;

				fiber.Initialize((PlatformFiber::Procedure)&JobManager::MainFiberWorker, sizeof(void*) * 4, fiberArguments);

				thread.Initialize((PlatformThread::Procedure)&JobManager::ThreadWorker, sizeof(void*) * 2, threadArguments);
				thread.SetCoreAffinity(i);
			}
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

			while (true)
			{
				uint8 priority = (uint8)Priority::High;

				while (true)
				{
					if (arguments->JobsQueues[priority].Pop(&task))
						break;

					if (priority-- == (uint8)Priority::Low)
						priority = (uint8)Priority::High;

					arguments->Thread->Sleep(1000);
				}

				TaskFiberWorkerArguments *fiberArguments = reinterpret_cast<TaskFiberWorkerArguments*>(AllocateMemory(&TaskFiberWorkerArgumentAllocator, 1));
				fiberArguments->MainFiber = arguments->Fiber;
				fiberArguments->Task = &task;

				Fiber *fiber = reinterpret_cast<Fiber*>(AllocateMemory(&FiberAllocator, 1));
				new (fiber) Fiber;
				fiber->Initialize(TaskFiberWorker, 32, fiberArguments);

				fiber->Switch();
			}
		}

		void JobManager::TaskFiberWorker(void *Arguments)
		{
			TaskFiberWorkerArguments *arguments = reinterpret_cast<TaskFiberWorkerArguments*>(Arguments);

			arguments->Task->Do();

			arguments->MainFiber->Switch();
		}
	}
}