// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <Common\ScopeGaurd.h>
#include <queue>

namespace Engine
{
	namespace Containers
	{
		//TODO: Implement Queue
		template<typename T>
		class ThreadSafeQueue
		{
		public:
			INLINE void Push(T& Value)
			{
				ScopeGaurd gaurd(m_Lock);

				m_Queue.push(Value);
			}

			INLINE bool Top(T* Value)
			{
				ScopeGaurd gaurd(m_Lock);

				if (m_Queue.empty())
					return false;

				*Value = m_Queue.front();

				return true;
			}

			INLINE bool Pop(T* Value)
			{
				ScopeGaurd gaurd(m_Lock);

				if (m_Queue.empty())
					return false;

				*Value = m_Queue.front();

				m_Queue.pop();

				return true;
			}

		private:
			std::queue<T> m_Queue;
			SpinLock m_Lock;
		};
	}
}

#endif