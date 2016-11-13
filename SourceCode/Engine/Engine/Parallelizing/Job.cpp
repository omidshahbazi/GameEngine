// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\Job.h>

namespace Engine
{
	namespace Parallelizing
	{
		Job::Job(Procedure Procedure) :
			m_Procedure(Procedure),
			m_IsFinished(false)
		{
		}

		void Job::Do(void)
		{
			m_Procedure();
			m_IsFinished = true;
		}
	}
}