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
		class IJob
		{
		public:
			virtual void Do(void) = 0;
		};

		template<typename R> class JobBase
		{
		public:
			typedef std::function<R(void)> F;

		public:
			JobBase(void) :
				m_Then(nullptr)
			{
			}

			template<typename Function, typename ...Parameters> Job<R> *Then(Function &&Function, Parameters&&... Arguments)
			{
			//	m_Then = [Function, Arguments...](){ Function(Arguments...); };
			//	return this;
			}

		private:
			std::function<void(void)> m_Then;
		};

		template<typename R> class Job : public IJob, public JobBase<R>
		{
		public:
			Job<R>(F &&Function) :
				m_Function(std::forward<F>(Function)),
				m_Finished(false)
			{
			}

			void Do(void) override
			{
				m_Result = m_Function();

				m_Finished = true;
			}

			bool IsFinished(void) const
			{
				return m_Finished;
			}

			const R &Get(void) const
			{
				return m_Result;
			}

		private:
			F m_Function;
			R m_Result;
			std::atomic<bool> m_Finished;
		};

		template<> class Job<void> : public IJob, public JobBase<void>
		{
		public:
			Job(F &&Function) :
				m_Function(std::forward<F>(Function)),
				m_Finished(false)
			{
			}

			void Do(void) override
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