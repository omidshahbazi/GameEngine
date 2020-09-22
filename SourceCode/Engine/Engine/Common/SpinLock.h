// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

namespace Engine
{
	namespace Common
	{
		class COMMON_API SpinLock
		{
		public:
			SpinLock(void);

			void Lock(void);
			bool TryLock(void);

			void Release(void);

		private:
			std::atomic_bool m_IsLocked;
		};
	}
}

#endif