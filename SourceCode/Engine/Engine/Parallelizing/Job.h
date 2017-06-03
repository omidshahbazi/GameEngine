// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <atomic>
#include <functional>

#ifndef JOB_H
#define JOB_H

namespace Engine
{
	namespace Parallelizing
	{

		template<typename R> class Job
		{
		public:
			typedef std::function<R(void)> F;

		public:
			Job<R>(F &&Function) :
				m_Function(std::forward<F>(Function)),
				m_Finished(false)
			{
			}

			void Do(void)
			{
				m_Result = m_Function();

				m_Finished = true;
			}

			bool IsFinished(void) const
			{
				return m_Finished;
			}

		private:
			F m_Function;
			R m_Result;
			std::atomic<bool> m_Finished;
		};

		template<> class Job<void>
		{
		public:
			typedef std::function<void(void)> F;

		public:
			Job(F &&Function) :
				m_Function(std::forward<F>(Function)),
				m_Finished(false)
			{
			}

			void Do(void)
			{
				m_Function();

				m_Finished = true;
			}

			bool IsFinished(void) const
			{
				return m_Finished;
			}

		private:
			F m_Function;
			std::atomic<bool> m_Finished;
		};
	}
}

#endif