// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Parallelizing\Private\MainFiberWorkerArguments.h>
#include <Parallelizing\Private\ThreadWorkerArguments.h>
#include <Containers\StringUtility.h>
#include <Threading\Fiber.h>
#include <Platform\PlatformFiber.h>

namespace Engine
{
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		using namespace Private;

		SINGLETON_DEFINITION(JobManager);

		const uint8 WORKER_FIBERS_COUNT = 128;

		JobManager::JobManager(void) :
			m_ThreadCount(0),
			m_Threads(nullptr),
			m_MainFibers(nullptr),
			m_WorkerFibersPtr(nullptr),
			m_ThreadArguments(nullptr),
			m_FiberArguments(nullptr),
			//JobQueue m_JobQueues[(uint8)Priority::High + 1];
			//FiberQueue m_WorkerFibers;
			m_IsWaitingTaskInfosProcessing(false),
			m_WaitingTaskInfos(ParallelizingAllocators::WaitingListAllocator)
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
				fbrArg.Fiber = &fiber;
				fbrArg.JobQueues = m_JobQueues;
				fbrArg.WorkerFiberQueue = &m_WorkerFibers;
				fbrArg.WaitingTaskInfos = &m_WaitingTaskInfos;
				fbrArg.IsWaitingTaskInfosProcessing = &m_IsWaitingTaskInfosProcessing;
				fbrArg.ShouldExit = false;

				fiber.Initialize((PlatformFiber::Procedure) & JobManager::MainFiberWorker, sizeof(void*) * 4, &fbrArg);

				thread.Initialize((PlatformThread::Procedure) & JobManager::ThreadWorker, sizeof(void*) * 2, &thdArg);
				thread.SetCoreAffinity(i);
				thread.SetName(String("Job Worker Threads ") + StringUtility::ToString<char8>(i + 1));
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
				m_FiberArguments[i].ShouldExit.exchange(true);

			for (int8 i = 0; i < m_ThreadCount; ++i)
				if (m_FiberArguments[i].ShouldExit.load())
					--i;

			ParallelizingAllocators::ThreadWorkerArgumentsAllocator_Deallocate(m_ThreadArguments);
			ParallelizingAllocators::MainFiberWorkerArgumentAllocator_Deallocate(m_FiberArguments);

			DeallocateMemory(ParallelizingAllocators::FiberAllocator, m_WorkerFibersPtr);
			DeallocateMemory(ParallelizingAllocators::FiberAllocator, m_MainFibers);

			for (uint8 i = 0; i < m_ThreadCount; ++i)
				Destruct(&m_Threads[i]);
			ParallelizingAllocators::ThreadAllocator_Deallocate(m_Threads);
		}

		void JobManager::AddJob(JobInfoHandle* Handle, Priority Priority)
		{
			m_JobQueues[(uint8)Priority].Push(Handle);
		}

		void JobManager::WaitFor(JobInfoHandle* Handle)
		{
			if (!PlatformFiber::IsRunningOnFiber())
				return;

			TaskFiberWorkerArguments* arguements = ReinterpretCast(TaskFiberWorkerArguments*, PlatformFiber::GetData());
			Assert(arguements != nullptr, "Fiber data is null");

			WaitingTaskInfo info = { arguements->Fiber, Handle };
			m_WaitingTaskInfos.Add(info);

			arguements->Fiber->SwitchBack();
		}

		bool JobManager::RunHandle(FiberQueue* WorkerFiberQueue, JobInfoHandle* Handle, Fiber* BaseFiber)
		{
			Fiber* fiber = nullptr;
			if (!WorkerFiberQueue->Pop(&fiber))
				return false;

			TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
			fiberArguments->Handle = Handle;
			fiberArguments->Fiber = fiber;
			fiberArguments->WorkerFiberQueue = WorkerFiberQueue;

			fiber->Deinitialize();
			fiber->Initialize(TaskFiberWorker, 32, fiberArguments);

			BaseFiber->SwitchTo(fiber);

			return true;
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

			while (!arguments->ShouldExit.load())
			{
				if (arguments->ShouldExit.load())
					break;

				if (!(*arguments->IsWaitingTaskInfosProcessing))
				{
					*arguments->IsWaitingTaskInfosProcessing = true;

					auto& infos = *arguments->WaitingTaskInfos;

					for (uint16 i = 0; i < infos.GetSize(); ++i)
					{
						auto& info = infos[i];

						if (!info.WaitingForHandle->m_IsFinished)
							continue;

						arguments->Fiber->SwitchTo(info.Fiber);

						infos.RemoveAt(i--);
					}

					*arguments->IsWaitingTaskInfosProcessing = false;
				}

				JobInfoHandle* handle = nullptr;

				int8 priority = (uint8)Priority::High;
				while (priority >= (uint8)Priority::Low)
				{
					if (arguments->JobQueues[priority].Pop(&handle))
						break;

					--priority;
				}

				if (handle == nullptr)
					continue;

				if (!RunHandle(arguments->WorkerFiberQueue, handle, arguments->Fiber))
				{
					arguments->JobQueues[priority].Push(handle);
					continue;
				}
			}

			arguments->ShouldExit.exchange(false);
		}

		void JobManager::TaskFiberWorker(void* Arguments)
		{
			TaskFiberWorkerArguments* arguments = ReinterpretCast(TaskFiberWorkerArguments*, Arguments);

			arguments->Handle->Do();

			JobManager::FiberQueue* fiberQueue = arguments->WorkerFiberQueue;
			Fiber* fiber = arguments->Fiber;

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(arguments);

			fiberQueue->Push(fiber);

			fiber->SwitchBack();
		}
	}
}