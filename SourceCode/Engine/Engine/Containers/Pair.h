// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			Pair(void)
			{
				Construct(&m_First);
				Construct(&m_Second);
			}

			Pair(const F& First, const S& Second) :
				m_First(First),
				m_Second(Second)
			{
			}

			Pair(const Pair<F, S>& Other) :
				m_First(Other.m_First),
				m_Second(Other.m_Second)
			{
			}

			Pair(Pair<F, S>&& Other)
			{
				Construct(&m_First);
				Construct(&m_Second);

				*this = std::move(Other);
			}

			~Pair(void)
			{
				Destruct(&m_First);
				Destruct(&m_Second);
			}

			INLINE void SetFirst(const F& First)
			{
				m_First = First;
			}

			INLINE void SetSecond(const S& Second)
			{
				m_Second = Second;
			}

			INLINE F& GetFirst(void)
			{
				return m_First;
			}

			INLINE S& GetSecond(void)
			{
				return m_Second;
			}

			INLINE const F& GetFirst(void) const
			{
				return m_First;
			}

			INLINE const S& GetSecond(void) const
			{
				return m_Second;
			}

			INLINE Pair<F, S>& operator=(const Pair<F, S>& Other)
			{
				m_First = Other.m_First;
				m_Second = Other.m_Second;

				return *this;
			}

			INLINE Pair<F, S>& operator=(Pair<F, S>&& Other)
			{
				m_First = Other.m_First;
				m_Second = Other.m_Second;

				return *this;
			}

		private:
			F m_First;
			S m_Second;
		};
	}
}

#endif