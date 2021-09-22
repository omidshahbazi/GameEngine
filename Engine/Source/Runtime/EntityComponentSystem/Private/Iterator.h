// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

namespace Engine
{
	namespace EntityComponentSystem
	{
		namespace Private
		{
			template<typename T>
			class Iterator
			{
			public:
				Iterator(T* Address = nullptr) :
					m_Address(Address)
				{
				}

				Iterator(const Iterator& Other) :
					m_Address(Other.Address)
				{
				}

				Iterator& operator= (const Iterator& Other)
				{
					m_Address = Other.m_Address;
					return *this;
				}

				Iterator& operator++ (void)
				{
					++m_Address;
					return *this;
				}

				Iterator& operator-- (void)
				{
					--m_Address;
					return *this;
				}

				bool operator== (const Iterator& Other)
				{
					return (m_Address == Other.m_Address);
				}

				bool operator!= (const Iterator& Other)
				{
					return !(*this == Other);
				}

				T& operator* (void) const
				{
					return *m_Address;
				}

			private:
				T* m_Address;
			};
		}
	}
}

#endif