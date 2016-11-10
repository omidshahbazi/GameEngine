// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef JOB_H
#define JOB_H

namespace Engine
{
	namespace Parallelizing
	{
		class Job
		{
		public:
			typedef void(Procedure)();

		public:
			Job(void);
		};
	}
}

#endif