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
		template<typename R> class JobInfo
		{
		public:
			typedef std::function<R(void)> F;

			JobInfo(F &&Function) :
				m_Function((Function))
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

			const R &Get(void) const
			{
				return m_Result;
			}

		private:
			F m_Function;
			std::atomic<bool> m_Finished;
			R m_Result;
		};

		template<> class JobInfo<void>
		{
		public:
			typedef std::function<void(void)> F;

			JobInfo(F &&Function) :
				m_Function((Function))
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

		template<typename R> class JobBase
		{
		public:
			JobBase(JobInfo<R> *Info) :
				m_Info(Info)
			{ }

			bool IsFinished(void) const
			{
				return m_Info->IsFinished();
			}

		protected:
			JobInfo<R> *m_Info;
		};

		template<typename R> class Job : public JobBase<R>
		{
		public:
			Job<R>(JobInfo<R> *Info) :
				JobBase<R>(Info)
			{
			}

			const R &Get(void) const
			{
				return m_Info->Get();
			}
		};

		template<> class Job<void> : public JobBase<void>
		{
		public:
			Job(JobInfo<void> *Info) :
				JobBase<void>(Info)
			{
			}
		};
	}
}

#endif