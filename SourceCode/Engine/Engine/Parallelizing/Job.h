// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <atomic>

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

			bool IsFinished(void)
			{
				return m_IsFinished;
			}

		private:
			Procedure m_Procedure;
			std::atomic<bool> m_IsFinished;
		};
	}
}

#endif