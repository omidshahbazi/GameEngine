// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_WORKER_ARGUMENTS_H
#define THREAD_WORKER_ARGUMENTS_H

#include <Threading\Thread.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace Threading;

	namespace Parallelizing
	{
		namespace Private
		{
			struct ThreadWorkerArguments
			{
			public:
				Thread * Thread;
				Fiber *Fiber;
			};
		}
	}
}

#endif