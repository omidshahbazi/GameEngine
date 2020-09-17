// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TASK_FIBER_WORKER_ARGUEMTNS_H
#define TASK_FIBER_WORKER_ARGUEMTNS_H

#include <Threading\Fiber.h>
#include <Parallelizing\JobInfo.h>

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
				Fiber* CurrentFiber;
				JobInfoHandle* Handle;
				JobManager::FiberQueue* WorkerFiberQueue;
			};
		}
	}
}

#endif