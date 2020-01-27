// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MAIN_FIBER_WORKER_ARGUMENTS_H
#define MAIN_FIBER_WORKER_ARGUMENTS_H

#include <Threading\Thread.h>
#include <Threading\Fiber.h>
#include <Parallelizing\JobManager.h>

namespace Engine
{
	using namespace Threading;

	namespace Parallelizing
	{
		namespace Private
		{
			struct MainFiberWorkerArguments
			{
			public:
				Thread * Thread;
				Fiber *MainFiber;
				JobManager::QueueType *JobsQueues;
				JobManager::FiberQueue *WorkerFiberQueue;
			};
		}
	}
}

#endif