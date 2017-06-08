// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
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
			JobManager::QueueType *Jobs;
		};

		FixedSizeAllocator ThreadAllocator("Thread Allocator", &Allocators::JobSystemAllocator, sizeof(Thread), 32);
		FixedSizeAllocator FiberAllocator("Fiber Allocator", &Allocators::JobSystemAllocator, sizeof(Thread), 32);
		FixedSizeAllocator ThreadWorkerArgumentsAllocator("Thread Worker Argument Allocator", &Allocators::JobSystemAllocator, sizeof(ThreadWorkerArguments), 32);
		FixedSizeAllocator MainFiberWorkerArgumentAllocator("Fiber Worker Arguments Allocator", &Allocators::JobSystemAllocator, sizeof(Fiber), 32);

		JobManager *JobManager::instance = nullptr;

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_ThreadCount = 2;
			m_Threads = reinterpret_cast<Thread*>(AllocateMemory(&ThreadAllocator, sizeof(Thread) * m_ThreadCount));
			m_Fibers = reinterpret_cast<Fiber*>(AllocateMemory(&FiberAllocator, sizeof(Fiber) * m_ThreadCount));

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];
				new (&thread) Thread;

				Fiber &fiber = m_Fibers[i];
				new (&fiber) Fiber;

				ThreadWorkerArguments *threadArguments = reinterpret_cast<ThreadWorkerArguments*>(AllocateMemory(&ThreadWorkerArgumentsAllocator, sizeof(ThreadWorkerArguments)));
				threadArguments->Thread = &thread;
				threadArguments->Fiber = &fiber;

				MainFiberWorkerArguments *fiberArguments = reinterpret_cast<MainFiberWorkerArguments*>(AllocateMemory(&MainFiberWorkerArgumentAllocator, sizeof(MainFiberWorkerArguments)));
				fiberArguments->Thread = threadArguments->Thread;
				fiberArguments->Fiber = &fiber;
				fiberArguments->Jobs = &m_Jobs;

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

			JobProcedure job = nullptr;

			while (true)
			{
				while (!arguments->Jobs->Pop(&job))
					arguments->Thread->Sleep(1000);

				job();
			}
		}
	}
}