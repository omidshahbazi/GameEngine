// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>

namespace Engine
{
	namespace Containers
	{
		template<typename T>
		class ThreadSafeQueue
		{
		public:
			INLINE void Push(T& Value)
			{
				std::lock_guard<std::mutex> gaurd(m_Lock);

				m_Queue.push(Value);
			}

			INLINE bool Pop(T* Value)
			{
				std::lock_guard<std::mutex> gaurd(m_Lock);

				if (m_Queue.empty())
					return false;

				*Value = m_Queue.front();
				m_Queue.pop();

				return true;
			}

		private:
			std::queue<T> m_Queue;
			std::mutex m_Lock;
		};
	}
}

#endif