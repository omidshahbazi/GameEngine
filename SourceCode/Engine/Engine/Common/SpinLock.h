// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

#ifdef DEBUG_MODE
#include <thread>
#endif

namespace Engine
{
	namespace Common
	{
		class COMMON_API SpinLock
		{
		public:
			SpinLock(void);

			bool IsLocked(void);

			void Lock(void);
			bool TryLock(void);

			void Release(void);

		private:
			std::atomic_bool m_IsLocked;

#ifdef DEBUG_MODE
			std::thread::id m_LastLockerThread;
#endif
		};
	}
}

#endif