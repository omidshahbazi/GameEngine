// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <Common\PrimitiveTypes.h>
#include <atomic>

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
			bool TryLock(uint16 ForSeconds = 0);

			void Release(void);

		private:
			std::atomic_bool m_IsLocked;
		};
	}
}

#endif