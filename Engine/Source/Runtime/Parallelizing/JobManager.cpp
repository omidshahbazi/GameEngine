// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>
#include <Containers\StringUtility.h>
#include <Containers\Exception.h>
#include <Threading\Fiber.h>
#include <Common\SpinLock.h>
#include <Platform\PlatformFiber.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
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
			m_MainFibers(nullptr)
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

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread& thread = m_Threads[i];
				Construct(&thread);

				Fiber& fiber = m_MainFibers[i];
				Construct(&fiber);

				fiber.Initialize([this, i](void*) { MainFiberWorker(i); });

				thread.Initialize([this, i](void*) { ThreadWorker(i); });
				thread.SetCoreAffinity(i);
				thread.SetName(String("Job Worker Threads ") + StringUtility::ToString<char8>(i + 1));
			}
		}

		JobManager::~JobManager(void)
		{
			for (int8 i = 0; i < m_ThreadCount; ++i)
				m_MainFibers[i].Shutdown().Wait();

			for (int8 i = 0; i < m_ThreadCount; ++i)
				m_Threads[i].Shutdown().Wait();

			for (auto fiber : m_WorkerFibers)
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
				THROW_IF_EXCEPTION(Categories::Parallelizing, arguements == nullptr, "Fiber data is null");

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
			Fiber& fiber = m_MainFibers[ArgumentIndex];

			PlatformFiber::ConvertThreadToFiber(nullptr);

			fiber.Switch();
		}

		void JobManager::MainFiberWorker(uint32 ArgumentIndex)
		{
			static SpinLock waitingListProcessingLock;

			Fiber& fiber = m_MainFibers[ArgumentIndex];

			while (!fiber.GetShouldExit())
			{
				PlatformThread::Sleep(1);

				if (waitingListProcessingLock.TryLock())
				{
					for (int16 i = 0; i < m_WaitingTaskInfos.GetSize(); ++i)
					{
						auto& info = m_WaitingTaskInfos[i];

						if (!info.WaitingForHandle->m_IsFinished)
							continue;

						fiber.SwitchTo(info.Fiber);

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

				if (!RunHandle(fiber, handle))
					m_JobQueues[priority].Enqueue(handle);
			}
		}

		bool JobManager::RunHandle(Fiber& Parent, JobInfoHandle* Handle)
		{
			Fiber* fiber = nullptr;
			if (m_WorkerFibers.Dequeue(&fiber))
				fiber->Shutdown(true);
			else
			{
				fiber = ParallelizingAllocators::FiberAllocator_Allocate<Fiber>();
				Construct(fiber);
			}

			TaskFiberWorkerArguments* fiberArguments = ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Allocate<TaskFiberWorkerArguments>();
			fiberArguments->Handle = Handle;
			fiberArguments->Fiber = fiber;

			fiber->Initialize([this, fiberArguments](void*) { TaskFiberWorker(*fiberArguments); }, 0, fiberArguments);

			Parent.SwitchTo(fiber);

			return true;
		}

		void JobManager::TaskFiberWorker(TaskFiberWorkerArguments& Arguments)
		{
			try
			{
				Arguments.Handle->Do();
			}
			catch (Exception& ex)
			{
				CoreDebugLogException(Categories::Parallelizing, ex);
			}
			catch (...)
			{
				CoreDebugLogError(Categories::Parallelizing, "Unhandle exception in running a job");
			}

			Arguments.Handle->Drop();

			Fiber* fiber = Arguments.Fiber;

			ParallelizingAllocators::TaskFiberWorkerArgumentAllocator_Deallocate(&Arguments);

			m_WorkerFibers.Enqueue(fiber);

			fiber->SwitchBack();
		}
	}
}