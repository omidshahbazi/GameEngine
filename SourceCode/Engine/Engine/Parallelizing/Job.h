// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Parallelizing\JobInfo.h>
#include <atomic>
#include <functional>

#ifndef JOB_H
#define JOB_H

namespace Engine
{
	namespace Parallelizing
	{
		template<typename R>
		class JobBase
		{
		public:
			JobBase(void) :
				m_Info(nullptr)
			{ }

			JobBase(JobInfo<R> *Info) :
				m_Info(Info)
			{
				m_Info->Grab();
			}

			JobBase(const JobBase &Other) :
				m_Info(Other.m_Info)
			{
				m_Info->Grab();
			}

			~JobBase(void)
			{
				m_Info->Drop();
			}

			JobBase<R> &operator = (const JobBase<R> &Other)
			{
				if (m_Info != nullptr)
					m_Info->Drop();

				m_Info = Other.m_Info;
				m_Info->Grab();

				return *this;
			}

			bool IsFinished(void) const
			{
				return m_Info->IsFinished();
			}

		protected:
			JobInfo<R> *m_Info;
		};

		template<typename R>
		class Job : public JobBase<R>
		{
		public:
			Job(void) :
				JobBase<R>()
			{ }

			Job(JobInfo<R> *Info) :
				JobBase<R>(Info)
			{ }

			const R &Get(void) const
			{
				return m_Info->Get();
			}
		};

		template<>
		class Job<void> : public JobBase<void>
		{
		public:
			Job(void) :
				JobBase<void>()
			{ }

			Job(JobInfo<void> *Info) :
				JobBase<void>(Info)
			{
			}
		};
	}
}

#endif