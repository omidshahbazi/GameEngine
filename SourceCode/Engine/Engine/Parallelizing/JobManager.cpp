// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Parallelizing\Private\MainFiberWorkerArguments.h>
#include <Parallelizing\Private\ThreadWorkerArguments.h>
#include <Containers\StringUtility.h>
#include <Threading\Fiber.h>
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

		std::mutex m_Lock;

		void Print(cstr Message, void* Address)
		{
			std::lock_guard<std::mutex> gaurd(m_Lock);

			std::cout << Message << Address << std::endl;
		}

		JobManager::JobManager(void) :
			m_ThreadCount(0),
			m_Threads(nullptr),
			m_MainFibers(nullptr),
			//m_WorkerFibersPtr(nullptr),
			m_ThreadArguments(nullptr),
			m_FiberArguments(nullptr),
			m_IsWaitingTaskInfosProcessing(false),
			m_RunningTaskCount(0)
		{
			ParallelizingAllocators::Create();

			//JobQueue m_JobQueues[(uint8)Priority::High + 1];
			//FiberQueue m_WorkerFibers;
			m_WaitingTaskInfos = WaitingTaskInfoList(ParallelizingAllocators::WaitingListAllocator, WORKER_FIBERS_COUNT);

			m_ThreadCount = PlatformThread::GetHardwareConcurrency();
			m_Threads = ParallelizingAllocators::ThreadAllocator_AllocateArray<Thread>(m_ThreadCount);
			m_MainFibers = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(m_ThreadCount);
			//m_WorkerFibersPtr = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(WORKER_FIBERS_COUNT);

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
				fbrArg.RunningTaskCount = &m_RunningTaskCount;

				fiber.Initialize((PlatformFiber::Procedure) & JobManager::MainFiberWorker, 0, &fbrArg);

				thread.Initialize((PlatformThread::Procedure) & JobManager::ThreadWorker, 0, &thdArg);
				thread.SetCoreAffinity(i);
				thread.SetName(String("Job Worker Threads ") + StringUtility::ToString<char8>(i + 1));
			}

			//for (uint8 i = 0; i < WORKER_FIBERS_COUNT; ++i)
			//{
			//	Fiber* fiber = &m_WorkerFibersPtr[i];

			//	Construct(fiber);

			//	m_WorkerFibers.Push(fiber);
			//}
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

			//DeallocateMemory(ParallelizingAllocators::FiberAllocator, m_WorkerFibersPtr);
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

			if (Handle->m_IsFinished)
				return;

			TaskFiberWorkerArguments* arguements = ReinterpretCast(TaskFiberWorkerArguments*, PlatformFiber::GetData());
			Assert(arguements != nullptr, "Fiber data is null");

			WaitingTaskInfo info = { arguements->Fiber, Handle };
			m_WaitingTaskInfos.Add(info);

			arguements->Fiber->SwitchBack();
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
			static std::mutex waitingLock;

			MainFiberWorkerArguments* arguments = ReinterpretCast(MainFiberWorkerArguments*, Arguments);

			while (!arguments->ShouldExit)
			{
				if (arguments->ShouldExit)
					break;

				PlatformThread::Sleep(1);


				//if (!arguments->IsWaitingTaskInfosProcessing->load(std::memory_order::memory_order_consume))
				{
					std::lock_guard<std::mutex> gaurd(waitingLock);

					//*arguments->IsWaitingTaskInfosProcessing = true;

					auto& infos = *arguments->WaitingTaskInfos;

					for (int16 i = 0; i < infos.GetSize(); ++i)
					{
						auto& info = infos[i];

						if (!info.WaitingForHandle->m_IsFinished)
							continue;

						Print("Resume ", info.Fiber);

						arguments->Fiber->SwitchTo(info.Fiber);

						infos.RemoveAt(i--);
					}

					//*arguments->IsWaitingTaskInfosProcessing = false;
				}

				//if (*arguments->RunningTaskCount >= HALF_WORKER_FIBERS_COUNT)
				//	continue;

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

				if (!RunHandle(arguments, handle))
					arguments->JobQueues[priority].Push(handle);
			}

			arguments->ShouldExit = false;
		}

		bool JobManager::RunHandle(MainFiberWorkerArguments* Arguments, JobInfoHandle* Handle)
		{
			Fiber* fiber = nullptr;
			if (!Arguments->WorkerFiberQueue->Pop(&fiber))
			{
				//if (*Arguments->RunningTaskCount >= WORKER_FIBERS_COUNT)
					//return false;

				fiber = ParallelizingAllocators::FiberAllocator_AllocateArray<Fiber>(1);
				Construct(fiber);

				Print("Create ", fiber);
			}

			Print("Run ", fiber);

			Handle->Grab();

			TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
			fiberArguments->Handle = Handle;
			fiberArguments->Fiber = fiber;
			fiberArguments->WorkerFiberQueue = Arguments->WorkerFiberQueue;
			fiberArguments->RunningTaskCount = Arguments->RunningTaskCount;

			fiber->Deinitialize();
			fiber->Initialize(TaskFiberWorker, 0, fiberArguments);

			Arguments->Fiber->SwitchTo(fiber);

			Handle->Drop();

			return true;
		}

		void JobManager::TaskFiberWorker(void* Arguments)
		{
			TaskFiberWorkerArguments* arguments = ReinterpretCast(TaskFiberWorkerArguments*, Arguments);

			AtomicUInt16& runningTaskCount = *arguments->RunningTaskCount;

			//++runningTaskCount;

			arguments->Handle->Do();

			//--runningTaskCount;

			JobManager::FiberQueue* fiberQueue = arguments->WorkerFiberQueue;
			Fiber* fiber = arguments->Fiber;

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(arguments);

			fiberQueue->Push(fiber);

			fiber->SwitchBack();
		}
	}
}