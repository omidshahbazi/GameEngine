// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <queue>
#include <mutex>

#pragma once
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

namespace Engine
{
	namespace Containers
	{
		template<typename T> class ThreadSafeQueue
		{
		public:
			void Push(T &Value)
			{
				std::lock_guard<std::mutex> gaurd(m_Lock);

				m_Queue.push(Value);
			}

			bool Pop(T *Value)
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