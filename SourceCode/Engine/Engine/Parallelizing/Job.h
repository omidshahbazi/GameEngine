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
			typedef void(*Procedure)(void);

		public:
			Job(Procedure Procedure);

			void Do(void);

		private:
			Procedure m_Procedure;
		};
	}
}

#endif