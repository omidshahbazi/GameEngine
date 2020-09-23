// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Parallelizing\Private\MainFiberWorkerArguments.h>
#include <Parallelizing\Private\ThreadWorkerArguments.h>
#include <Containers\StringUtility.h>
#include <Threading\Fiber.h>
#include <Common\SpinLock.h>
#include <Platform\PlatformFiber.h>

#include <iostream>

namespace Engine
{
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		using namespace Private;

		SINGLETON_DEFINITION(JobManager);

		const uint8 WORKER_FIBERS_COUNT = 255;

		JobManager::JobManager(void) :
			m_ThreadCount(0),
			m_Threads(nullptr),
			m_MainFibers(nullptr),
			m_ThreadArguments(nullptr),
			m_FiberArguments(nullptr)
		{
			ParallelizingAllocators::Create();

			int8 priority = (uint8)Priority::Low;
			while (priority <= (uint8)Priority::High)
				Construct(&m_JobQueues[priority++], ParallelizingAllocators::WaitingListAllocator);
			Construct(&m_WorkerFibers, ParallelizingAllocators::WaitingListAllocator);
			Construct(&m_WaitingTaskInfos, ParallelizingAllocators::WaitingListAllocator, WORKER_FIBERS_COUNT);

			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = ParallelizingAllocators::ThreadAllocator_AllocateArray<Thread>(m_ThreadCount);
			m_MainFibers = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(m_ThreadCount);

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
				fbrArg.Fiber = &fiber;
				fbrArg.JobQueues = m_JobQueues;
				fbrArg.WorkerFiberQueue = &m_WorkerFibers;
				fbrArg.WaitingTaskInfos = &m_WaitingTaskInfos;
				fbrArg.ShouldExit = false;

				fiber.Initialize((PlatformFiber::Procedure) & JobManager::MainFiberWorker, 0, &fbrArg);

				thread.Initialize((PlatformThread::Procedure) & JobManager::ThreadWorker, 0, &thdArg);
				thread.SetCoreAffinity(i);
				thread.SetName(String("Job Worker Threads ") + StringUtility::ToString<char8>(i + 1));
			}
		}

		JobManager::~JobManager(void)
		{
			for (uint8 i = 0; i < m_ThreadCount; ++i)
				m_FiberArguments[i].ShouldExit = true;

			for (int8 i = 0; i < m_ThreadCount; ++i)
				if (m_FiberArguments[i].ShouldExit)
					--i;

			ParallelizingAllocators::ThreadWorkerArgumentsAllocator_Deallocate(m_ThreadArguments);
			ParallelizingAllocators::MainFiberWorkerArgumentAllocator_Deallocate(m_FiberArguments);

			for each (auto fiber in m_WorkerFibers)
				ParallelizingAllocators::FiberAllocator_Deallocate(fiber);

			DeallocateMemory(ParallelizingAllocators::FiberAllocator, m_MainFibers);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_Threads[i]);
			ParallelizingAllocators::ThreadAllocator_Deallocate(m_Threads);
		}

		void JobManager::AddJob(JobInfoHandle* Handle, Priority Priority)
		{
			Handle->Grab();

			m_JobQueues[(uint8)Priority].Enqueue(Handle);
		}

		void JobManager::WaitFor(JobInfoHandle* Handle)
		{
			if (PlatformFiber::IsRunningOnFiber())
			{
				if (Handle->m_IsFinished)
					return;

				TaskFiberWorkerArguments* arguements = ReinterpretCast(TaskFiberWorkerArguments*, PlatformFiber::GetData());
				Assert(arguements != nullptr, "Fiber data is null");

				WaitingTaskInfo info = { arguements->Fiber, Handle };
				m_WaitingTaskInfos.Add(info);

				arguements->Fiber->SwitchBack();

				return;
			}

			while (!Handle->m_IsFinished)
				PlatformThread::Sleep(1);
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
			static SpinLock waitingListProcessingLock;

			MainFiberWorkerArguments* arguments = ReinterpretCast(MainFiberWorkerArguments*, Arguments);

			while (!arguments->ShouldExit)
			{
				if (arguments->ShouldExit)
					break;

				PlatformThread::Sleep(1);

				if (waitingListProcessingLock.TryLock())
				{
					auto& infos = *arguments->WaitingTaskInfos;

					for (int16 i = 0; i < infos.GetSize(); ++i)
					{
						auto& info = infos[i];

						if (!info.WaitingForHandle->m_IsFinished)
							continue;

						arguments->Fiber->SwitchTo(info.Fiber);

						infos.RemoveAt(i--);
					}

					waitingListProcessingLock.Release();
				}

				JobInfoHandle* handle = nullptr;

				int8 priority = (uint8)Priority::High;
				while (priority >= (uint8)Priority::Low)
				{
					if (arguments->JobQueues[priority].Dequeue(&handle))
						break;

					--priority;
				}

				if (handle == nullptr)
					continue;

				if (!RunHandle(arguments, handle))
					arguments->JobQueues[priority].Enqueue(handle);
			}

			arguments->ShouldExit = false;
		}

		bool JobManager::RunHandle(MainFiberWorkerArguments* Arguments, JobInfoHandle* Handle)
		{
			Fiber* fiber = nullptr;
			if (!Arguments->WorkerFiberQueue->Dequeue(&fiber))
			{
				fiber = ParallelizingAllocators::FiberAllocator_Allocate<Fiber>();
				Construct(fiber);
			}

			TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
			fiberArguments->Handle = Handle;
			fiberArguments->Fiber = fiber;
			fiberArguments->WorkerFiberQueue = Arguments->WorkerFiberQueue;

			fiber->Deinitialize();
			fiber->Initialize(TaskFiberWorker, 0, fiberArguments);

			Arguments->Fiber->SwitchTo(fiber);

			return true;
		}

		void JobManager::TaskFiberWorker(void* Arguments)
		{
			TaskFiberWorkerArguments* arguments = ReinterpretCast(TaskFiberWorkerArguments*, Arguments);

			arguments->Handle->Do();

			arguments->Handle->Drop();

			JobManager::FiberQueue* fiberQueue = arguments->WorkerFiberQueue;
			Fiber* fiber = arguments->Fiber;

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(arguments);

			fiberQueue->Enqueue(fiber);

			fiber->SwitchBack();
		}
	}
}