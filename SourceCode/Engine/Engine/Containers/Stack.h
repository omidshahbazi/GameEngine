// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STACK_H
#define STACK_H

#include <stack>

namespace Engine
{
	namespace Containers
	{
		//TODO: Implement Stack
		template<typename T>
		class Stack : public std::stack<T>
		{
		public:
			INLINE void Push(const T& Value)
			{
				push(Value);
			}

			INLINE T& Fetch(void)
			{
				return top();
			}

			INLINE void Pop(void)
			{
				pop();
			}

			INLINE T FetchAndPop(void)
			{
				T value = Fetch();
				Pop();
				return value;
			}

			INLINE uint32 GetSize(void) const
			{
				return size();
			}
		};
	}
}

#endif