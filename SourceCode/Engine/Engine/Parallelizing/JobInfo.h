// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_INFO_H
#define JOB_INFO_H

#include <Parallelizing\Private\ParallelizingAllocators.h>
#include <Common\PrimitiveTypes.h>
#include <functional>

namespace Engine
{
	using namespace Common;

	namespace Parallelizing
	{
		class JobManager;

		template<typename R>
		class JobBase;

		template<typename R>
		class Job;

		using namespace Private;

		class JobInfoHandle
		{
			friend class JobManager;

			template<typename R>
			friend class JobBase;

		protected:
			virtual void Do(void) = 0;

			INLINE void Grab(void)
			{
				++m_ReferenceCount;
			}

			INLINE void Drop(void)
			{
				if (--m_ReferenceCount != 0)
					return;

				ParallelizingAllocators::JobAllocator_Deallocate(this);
			}

		protected:
			AtomicBool m_IsFinished;
			AtomicUInt16 m_ReferenceCount;
		};

		template<typename R>
		class JobInfoBase : public JobInfoHandle
		{
			friend class JobManager;

			template<typename T> friend class JobBase;
			template<typename T> friend class Job;

		public:
			typedef std::function<R(void)> F;

		protected:
			JobInfoBase(F&& Function) :
				m_Function(Function)
			{ }

		protected:
			F m_Function;
		};

		template<typename R>
		class JobInfo : public JobInfoBase<R>
		{
			friend class JobManager;

			template<typename R>
			friend class Job;

		protected:
			JobInfo(F&& Function) :
				JobInfoBase<R>(std::forward<F>(Function))
			{
				Grab();
			}

			void Do(void) override
			{
				m_Result = m_Function();

				m_IsFinished = true;

				Drop();
			}

		private:
			R m_Result;
		};

		template<>
		class JobInfo<void> : public JobInfoBase<void>
		{
			friend class JobManager;

		protected:
			JobInfo(F&& Function) :
				JobInfoBase<void>(std::forward<F>(Function))
			{
				Grab();
			}

			void Do(void) override
			{
				m_Function();

				m_IsFinished = true;

				Drop();
			}
		};
	}
}

#endif