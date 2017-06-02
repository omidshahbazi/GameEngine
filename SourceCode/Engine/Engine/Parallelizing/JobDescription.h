// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#include <atomic>
#include <functional>

#ifndef JOB_DESCRIPTION_H
#define JOB_DESCRIPTION_H

namespace Engine
{
	namespace Parallelizing
	{
		class PARALLELIZING_API JobDescription
		{
		public:
			typedef std::function<void(void)> Procedure;

		public:
			JobDescription(Procedure Procedure, void *Arguments = nullptr);

			void Do(void);

			bool IsFinished(void) const
			{
				return m_Finished;
			}

		private:
			Procedure m_Procedure;
			void *m_Arguments;
			std::atomic<bool> m_Finished;
		};
	}
}

#endif