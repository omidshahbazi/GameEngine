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
				thdArg.Fiber = &fiber;

				MainFiberWorkerArguments& fbrArg = m_FiberArguments[i];
				fbrArg.Fiber = &fiber;
				fbrArg.ShouldExit = false;

				fiber.Initialize([this, i](void*) { MainFiberWorker(i); });

				thread.Initialize([this, i](void*) { ThreadWorker(i); });
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

		void JobManager::ThreadWorker(uint32 ArgumentIndex)
		{
			ThreadWorkerArguments& arguments = m_ThreadArguments[ArgumentIndex];

			Fiber* fiber = arguments.Fiber;

			fiber->ConvertThreadToFiber(nullptr);

			fiber->Switch();
		}

		void JobManager::MainFiberWorker(uint32 ArgumentIndex)
		{
			static SpinLock waitingListProcessingLock;

			MainFiberWorkerArguments& arguments = m_FiberArguments[ArgumentIndex];

			while (!arguments.ShouldExit)
			{
				if (arguments.ShouldExit)
					break;

				PlatformThread::Sleep(1);

				if (waitingListProcessingLock.TryLock())
				{
					for (int16 i = 0; i < m_WaitingTaskInfos.GetSize(); ++i)
					{
						auto& info = m_WaitingTaskInfos[i];

						if (!info.WaitingForHandle->m_IsFinished)
							continue;

						arguments.Fiber->SwitchTo(info.Fiber);

						m_WaitingTaskInfos.RemoveAt(i--);
					}

					waitingListProcessingLock.Release();
				}

				JobInfoHandle* handle = nullptr;

				int8 priority = (uint8)Priority::High;
				while (priority >= (uint8)Priority::Low)
				{
					if (m_JobQueues[priority].Dequeue(&handle))
						break;

					--priority;
				}

				if (handle == nullptr)
					continue;

				if (!RunHandle(arguments, handle))
					m_JobQueues[priority].Enqueue(handle);
			}

			arguments.ShouldExit = false;
		}

		bool JobManager::RunHandle(MainFiberWorkerArguments& Arguments, JobInfoHandle* Handle)
		{
			Fiber* fiber = nullptr;
			if (!m_WorkerFibers.Dequeue(&fiber))
			{
				fiber = ParallelizingAllocators::FiberAllocator_Allocate<Fiber>();
				Construct(fiber);
			}

			TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
			fiberArguments->Handle = Handle;
			fiberArguments->Fiber = fiber;

			fiber->Deinitialize();
			fiber->Initialize([this, fiberArguments](void*) { TaskFiberWorker(*fiberArguments); }, 0, fiberArguments);

			Arguments.Fiber->SwitchTo(fiber);

			return true;
		}

		void JobManager::TaskFiberWorker(TaskFiberWorkerArguments& Arguments)
		{
			Arguments.Handle->Do();

			Arguments.Handle->Drop();

			Fiber* fiber = Arguments.Fiber;

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(&Arguments);

			m_WorkerFibers.Enqueue(fiber);

			fiber->SwitchBack();
		}
	}
}