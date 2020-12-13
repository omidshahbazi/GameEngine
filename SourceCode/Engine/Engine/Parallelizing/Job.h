// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_H
#define JOB_H

#include <Parallelizing\JobInfo.h>
#include <Platform\PlatformThread.h>

namespace Engine
{
	using namespace Platform;

	namespace Parallelizing
	{
		class JobManager;

		template<typename R>
		class JobBase
		{
			friend class JobManager;

		public:
			JobBase(void) :
				m_Info(nullptr)
			{ }

			JobBase(const JobBase& Other) :
				m_Info(Other.m_Info)
			{
				m_Info->Grab();
			}

		protected:
			JobBase(JobInfo<R>* Info) :
				m_Info(Info)
			{
				m_Info->Grab();
			}

			~JobBase(void)
			{
				m_Info->Drop();
			}

		public:
			INLINE JobBase<R>& operator = (const JobBase<R>& Other)
			{
				if (m_Info != nullptr)
					m_Info->Drop();

				m_Info = Other.m_Info;
				m_Info->Grab();

				return *this;
			}

			INLINE bool IsFinished(void) const
			{
				return m_Info->IsFinished();
			}

		protected:
			JobInfo<R>* m_Info;
		};

		template<typename R>
		class Job : public JobBase<R>
		{
			friend class JobManager;

		public:
			Job(void) :
				JobBase<R>()
			{ }

		private:
			Job(JobInfo<R>* Info) :
				JobBase<R>(Info)
			{ }

		public:
			const R& Get(void) const
			{
				return JobBase<R>::m_Info->m_Result;
			}
		};

		template<>
		class Job<void> : public JobBase<void>
		{
			friend class JobManager;

		public:
			Job(void) :
				JobBase<void>()
			{ }

		private:
			Job(JobInfo<void>* Info) :
				JobBase<void>(Info)
			{
			}
		};
	}
}

#endif