// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TASK_FIBER_WORKER_ARGUEMTNS_H
#define TASK_FIBER_WORKER_ARGUEMTNS_H

#include <Threading\Fiber.h>
#include <Parallelizing\Task.h>

namespace Engine
{
	using namespace Threading;

	namespace Parallelizing
	{
		namespace Private
		{
			struct TaskFiberWorkerArguments
			{
			public:
				Fiber * MainFiber;
				Fiber * CurrentFiber;
				Task *Task;
				JobManager::FiberQueue *WorkerFiberQueue;
			};
		}
	}
}

#endif