// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <Common\SpinLock.h>

namespace Engine
{
	namespace Common
	{
		class COMMON_API ScopeGuard
		{
		public:
			ScopeGuard(SpinLock& Lock);
			~ScopeGuard(void);

			ScopeGuard(const SpinLock&) = delete;
			ScopeGuard& operator=(const SpinLock&) = delete;

		private:
			SpinLock& m_Lock;
		};
	}
}

#endif