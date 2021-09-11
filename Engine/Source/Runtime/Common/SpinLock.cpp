// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\SpinLock.h>
#include <chrono>

namespace Engine
{
	namespace Common
	{
		SpinLock::SpinLock(void) :
			m_IsLocked(false)
		{
		}

		bool SpinLock::IsLocked(void)
		{
			return m_IsLocked.load(std::memory_order_relaxed);
		}

		void SpinLock::Lock(void)
		{
			while (true)
			{
				if (!m_IsLocked.exchange(true, std::memory_order_acquire))
					break;

				while (m_IsLocked.load(std::memory_order_relaxed));
			}
		}

		bool SpinLock::TryLock(uint16 ForSeconds)
		{
			auto startTime = std::chrono::high_resolution_clock::now();

			bool result = false;
			while (true)
			{
				if (result = !m_IsLocked.exchange(true, std::memory_order_acquire))
					break;

				while (result = m_IsLocked.load(std::memory_order_relaxed))
				{
					if (ForSeconds <= (std::chrono::high_resolution_clock::now() - startTime).count())
						break;
				}
			}

			return result;
		}

		void SpinLock::Release(void)
		{
			bool expected = true;

			m_IsLocked.compare_exchange_weak(expected, false, std::memory_order::memory_order_release);
		}
	}
}