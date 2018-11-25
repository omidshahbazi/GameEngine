// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STACK_H
#define STACK_H

#include <stack>

namespace Engine
{
	namespace Containers
	{
		template<typename T>
		class Stack : public std::stack<T>
		{
		public:
			void Push(const T &Value)
			{
				push(Value);
			}

			T &Fetch(void)
			{
				return top();
			}

			void Pop(void)
			{
				pop();
			}

			T FetchAndPop(void)
			{
				T value = Fetch();
				Pop();
				return value;
			}

			uint32 GetSize(void) const
			{
				return size();
			}
		};
	}
}

#endif