// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PAIR_H
#define PAIR_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace Containers
	{
		template<typename F, typename S>
		class Pair
		{
		public:
			typedef F FirstType;
			typedef S SecondType;

		public:
			Pair(void) :
				m_First(default(F)),
				m_Second(default(S))
			{
			}

			Pair(const F &First, const S &Second) :
				m_First(First),
				m_Second(Second)
			{
			}

			Pair(const Pair<F, S> &Other) :
				m_First(Other.m_First),
				m_Second(Other.m_Second)
			{
			}

			Pair(Pair<F, S> &&Other) :
				m_First(Other.m_First),
				m_Second(Other.m_Second)
			{
			}

			Pair<F, S> &operator=(const Pair<F, S> &Other)
			{
				m_First = Other.m_First;
				m_Second = Other.m_Second;
				return *this;
			}

			Pair<F, S> &operator=(Pair<F, S> &&Other)
			{
				m_First = Other.m_First;
				m_Second = Other.m_Second;
				return *this;
			}

			INLINE void SetFirst(const F &First)
			{
				m_First = First;
			}

			INLINE void GetSecond(const S &Second)
			{
				m_Second = Second;
			}

			INLINE F &GetFirst(void)
			{
				return m_First;
			}

			INLINE S &GetSecond(void)
			{
				return m_Second;
			}

			INLINE const F &GetFirst(void) const
			{
				return m_First;
			}

			INLINE const S &GetSecond(void) const
			{
				return m_Second;
			}

		private:
			F m_First;
			S m_Second;
		};
	}
}

#endif