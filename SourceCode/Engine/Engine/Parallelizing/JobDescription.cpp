// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\JobDescription.h>

namespace Engine
{
	namespace Parallelizing
	{
		JobDescription::JobDescription(Procedure Procedure, void *Arguments) :
			m_Procedure(Procedure),
			m_Arguments(Arguments),
			m_Finished(false)
		{
		}

		void JobDescription::Do(void)
		{
			m_Procedure(m_Arguments);
			m_Finished = true;
		}
	}
}