// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobManager.h>
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Platform;
	using namespace Threading;

	namespace Parallelizing
	{
		JobManager::JobManager(void)
		{
			m_ThreadCount = Multithreading::GetHardwareConcurrency();
			m_Threads = new Thread[m_ThreadCount];

			for (uint8 i = 0; i < m_ThreadCount; ++i)
			{
				Thread &thread = m_Threads[i];

				thread.Initialize((Multithreading::Procedure)&JobManager::ThreadWorker, 64, &thread);
				thread.SetCoreAffinity(i);
			}
		}

		Job *JobManager::Add(Job::Procedure *Procedure)
		{
			//threadManager.GetThread(0).Do(Procedure);

			return nullptr;
		}

		void JobManager::ThreadWorker(void *Arguments)
		{
			Thread *thread = reinterpret_cast<Thread*>(Arguments);

			//whiel (thread->)
		}
	}
}