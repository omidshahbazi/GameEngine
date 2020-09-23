// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <Common\ScopeGaurd.h>
#include <Containers\Queue.h>

namespace Engine
{
	namespace Containers
	{
		//TODO: Implement Thread safe
		template<typename T>
		class ThreadSafeQueue
		{
		public:
			INLINE void Push(T& Value)
			{
				ScopeGaurd gaurd(m_Lock);

				m_Queue.Enqueue(Value);
			}

			INLINE bool Top(T* Value)
			{
				ScopeGaurd gaurd(m_Lock);

				if (m_Queue.GetSize() == 0)
					return false;

				m_Queue.Peek(Value);

				return true;
			}

			INLINE bool Pop(T* Value)
			{
				ScopeGaurd gaurd(m_Lock);

				if (m_Queue.GetSize() == 0)
					return false;

				m_Queue.Dequeue(Value);

				return true;
			}

		private:
			Queue<T> m_Queue;
			SpinLock m_Lock;
		};
	}
}

#endif