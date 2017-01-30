// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\JobDescription.h>

#ifndef JOB_H
#define JOB_H

namespace Engine
{
	namespace Parallelizing
	{
		class PARALLELIZING_API Job
		{
		public:
			Job(JobDescription *Description);

			void Do(void);

		private:
			JobDescription *m_Description;
		};
	}
}

#endif