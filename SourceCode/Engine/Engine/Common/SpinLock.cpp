// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\SpinLock.h>

namespace Engine
{
	namespace Common
	{
		SpinLock::SpinLock(void) :
			m_IsLocked(false)
		{
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

		bool SpinLock::TryLock(void)
		{
			return (!m_IsLocked.load(std::memory_order_relaxed) && !m_IsLocked.exchange(true, std::memory_order_acquire));
		}

		void SpinLock::Release(void)
		{
			bool expected = true;

			m_IsLocked.compare_exchange_weak(expected, false, std::memory_order::memory_order_release);
		}
	}
}