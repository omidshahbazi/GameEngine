// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Threading\Fiber.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

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
			ThreadSafeQueue<Job*> *Jobs;
		};

		struct JobFiberWorkerArguments
		{
		public:
			Fiber *ParentFiber;
			Job *Job;
		};

		FixedSizeAllocator threadAllocator(sizeof(Thread) + sizeof(JobFiberWorkerArguments), 100);
		FixedSizeAllocator threadWorkerArgumentsAllocator(sizeof(ThreadWorkerArguments) + sizeof(JobFiberWorkerArguments), 100);
		FixedSizeAllocator fiberAllocator(sizeof(Fiber) + sizeof(JobFiberWorkerArguments), 100);
		FixedSizeAllocator mainFiberWorkerArgumentAllocator(sizeof(Fiber) + sizeof(JobFiberWorkerArguments), 100);
		FixedSizeAllocator jobAllocator(sizeof(Job), 1000);
		FixedSizeAllocator jobDescriptionAllocator(sizeof(JobDescription), 1000);

		void AllocateFiber(Fiber **FiberAddress, JobFiberWorkerArguments **ArgumentsAddress)
		{
			byte *address = fiberAllocator.Allocate(1);

			*FiberAddress = reinterpret_cast<Fiber*>(address);
			*ArgumentsAddress = reinterpret_cast<JobFiberWorkerArguments*>(address + sizeof(Fiber));
		}

		JobManager *JobManager::instance = nullptr;

		JobManager::JobManager(void)
		{
			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = reinterpret_cast<Thread*>(threadAllocator.Allocate(sizeof(Thread) * m_ThreadCount));
			m_Fibers = reinterpret_cast<Fiber*>(fiberAllocator.Allocate(sizeof(Fiber) * m_ThreadCount));

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];
				new (&thread) Thread;

				Fiber &fiber = m_Fibers[i];
				new (&fiber) Fiber;

				ThreadWorkerArguments *threadArguments = reinterpret_cast<ThreadWorkerArguments*>(threadWorkerArgumentsAllocator.Allocate(sizeof(ThreadWorkerArguments)));
				threadArguments->Thread = &thread;
				threadArguments->Fiber = &fiber;

				MainFiberWorkerArguments *fiberArguments = reinterpret_cast<MainFiberWorkerArguments*>(mainFiberWorkerArgumentAllocator.Allocate(sizeof(MainFiberWorkerArguments)));
				fiberArguments->Thread = threadArguments->Thread;
				fiberArguments->Fiber = &fiber;
				fiberArguments->Jobs = &m_Jobs;

				fiber.Initialize((PlatformFiber::Procedure)&JobManager::MainFiberWorker, sizeof(void*) * 4, fiberArguments);

				thread.Initialize((PlatformThread::Procedure)&JobManager::ThreadWorker, sizeof(void*) * 2, threadArguments);
				thread.SetCoreAffinity(i);
			}
		}

		void JobManager::Add(JobDescription *Description)
		{
			Job *job = reinterpret_cast<Job*>(jobAllocator.Allocate(1));
			new (job) Job(Description);
			m_Jobs.Push(job);
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

			Job *job = nullptr;

			while (true)
			{
				while (!arguments->Jobs->Pop(&job))
					arguments->Thread->Sleep(1000);

				Fiber *fiber = nullptr;
				JobFiberWorkerArguments *fiberArguments = nullptr;
				AllocateFiber(&fiber, &fiberArguments);

				fiberArguments->ParentFiber = arguments->Fiber;
				fiberArguments->Job = job;

				new (fiber) Fiber;
				fiber->Initialize((PlatformFiber::Procedure)&JobManager::JobFiberWorker, 1, fiberArguments);
				fiber->Switch();

				fiberAllocator.Deallocate((byte*)fiber);
			}
		}

		void JobManager::JobFiberWorker(void *Arguments)
		{
			JobFiberWorkerArguments *arguments = reinterpret_cast<JobFiberWorkerArguments*>(Arguments);

			arguments->Job->Do();

			arguments->ParentFiber->Switch();
		}

		JobDescription *CreateJobDescription(JobDescription::Procedure Procedure, void *Arguments)
		{
			JobDescription *job = reinterpret_cast<JobDescription*>(jobDescriptionAllocator.Allocate(1));
			new (job) JobDescription(Procedure, Arguments);
			return job;
		}

		JobDescription *AddJob(JobDescription::Procedure Procedure, void *Arguments)
		{
			JobDescription *job = CreateJobDescription(Procedure, Arguments);
			JobManager::GetInstance().Add(job);
			return job;
		}
	}
}