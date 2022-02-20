// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\ScopeGuard.h>

#include <iostream>
#include <thread>

namespace Engine
{
	namespace Common
	{
		ScopeGuard::ScopeGuard(SpinLock& Gaurd) :
			m_Lock(Gaurd)
		{
			m_Lock.Lock();
		}

		ScopeGuard::~ScopeGuard(void)
		{
			m_Lock.Release();
		}
	}
}