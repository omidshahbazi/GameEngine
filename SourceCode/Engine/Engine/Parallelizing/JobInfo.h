// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JOB_INFO_H
#define JOB_INFO_H

#include <Common\PrimitiveTypes.h>
#include <functional>

namespace Engine
{
	using namespace Common;

	namespace Parallelizing
	{
		template<typename R>
		class JobInfoBase
		{
			template<typename T> friend class JobBase;
			template<typename T> friend class Job;

		public:
			typedef std::function<R(void)> F;

			JobInfoBase(F&& Function) :
				m_Function(Function),
				m_Finished(false),
				m_ReferenceCount(0)
			{ }

			virtual void Do(void) = 0;

			INLINE bool IsFinished(void) const
			{
				return m_Finished;
			}

		protected:
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
			F m_Function;
			AtomicBool m_Finished;
			AtomicUInt16 m_ReferenceCount;
		};

		template<typename R>
		class JobInfo : public JobInfoBase<R>
		{
		public:
			JobInfo(F&& Function) :
				JobInfoBase<R>(std::forward<F>(Function))
			{
				Grab();
			}

			void Do(void) override
			{
				m_Result = m_Function();

				m_Finished = true;

				Drop();
			}

			INLINE const R& Get(void) const
			{
				return m_Result;
			}

		private:
			R m_Result;
		};

		template<>
		class JobInfo<void> : public JobInfoBase<void>
		{
		public:
			JobInfo(F&& Function) :
				JobInfoBase<void>(std::forward<F>(Function))
			{
				Grab();
			}

			void Do(void) override
			{
				m_Function();

				m_Finished = true;

				Drop();
			}
		};
	}
}

#endif