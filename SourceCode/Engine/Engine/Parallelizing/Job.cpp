// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\Job.h>

namespace Engine
{
	namespace Parallelizing
	{
		Job::Job(JobDescription *Description) :
			m_Description(Description)
		{
		}	

		void Job::Do(void)
		{
			m_Description->Do();
		}
	}
}