// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Threading\Thread.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace Threading;
	using namespace Platform;

	namespace Parallelizing
	{
		struct WorkerArguments
		{
		public:
			Thread *Thread;
			ThreadSafeQueue<Job*> *Jobs;
		};

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = new Thread[m_ThreadCount];

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];

				WorkerArguments *arguments = new WorkerArguments();
				arguments->Thread = &thread;
				arguments->Jobs = &m_Jobs;

				thread.Initialize((PlatformThread::Procedure)&JobManager::ThreadWorker, 16, arguments);
				thread.SetCoreAffinity(i);
			}
		}

		Job *JobManager::Add(Job::Procedure Procedure)
		{
			Job *job = new Job(Procedure);
			m_Jobs.Push(job);

			return nullptr;
		}

		void  FiberWorker(void *Arguments)
		{
			Job *job = reinterpret_cast<Job*>(Arguments);

			job->Do();

			//main fiber per thread and a loop for any of them
		}

		void JobManager::ThreadWorker(void *Arguments)
		{
			//JobAllocator
			//FiberAllocator

			WorkerArguments *arguments = reinterpret_cast<WorkerArguments*>(Arguments);
			Job *job = nullptr;

			while (true)
			{
				while (!arguments->Jobs->Pop(&job))
					arguments->Thread->Sleep(100);

				Fiber *fiber = new Fiber(FiberWorker, 512, job);

				PlatformFiber::ConvertThreadToFiber(nullptr);

				fiber->Switch();
			}
		}
	}
}