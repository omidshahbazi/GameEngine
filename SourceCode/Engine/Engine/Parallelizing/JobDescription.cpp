// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobDescription.h>

namespace Engine
{
	namespace Parallelizing
	{
		JobDescription::JobDescription(Procedure &&Procedure) :
			m_Procedure(std::forward<JobDescription::Procedure>(Procedure)),
			m_Finished(false)
		{
		}

		void JobDescription::Do(void)
		{
			m_Procedure();
			m_Finished = true;
		}
	}
}