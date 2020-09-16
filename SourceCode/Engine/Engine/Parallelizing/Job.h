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
		template<typename R>
		class JobBase
		{
		public:
			JobBase(void) :
				m_Info(nullptr)
			{ }

			JobBase(const JobBase& Other) :
				m_Info(Other.m_Info)
			{
				m_Info->Grab();
			}

			JobBase(JobInfo<R>* Info) :
				m_Info(Info)
			{
				m_Info->Grab();
			}

			~JobBase(void)
			{
				m_Info->Drop();
			}

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

			INLINE void Wait(void) const
			{
				while (!m_Info->IsFinished())
					PlatformThread::Sleep(1);
			}

		protected:
			JobInfo<R>* m_Info;
		};

		template<typename R>
		class Job : public JobBase<R>
		{
		public:
			Job(void) :
				JobBase<R>()
			{ }

			Job(JobInfo<R>* Info) :
				JobBase<R>(Info)
			{ }

			const R& Get(void) const
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

			Job(JobInfo<void>* Info) :
				JobBase<void>(Info)
			{
			}
		};
	}
}

#endif