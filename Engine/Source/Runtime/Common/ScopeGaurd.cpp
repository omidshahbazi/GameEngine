// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\ScopeGaurd.h>

#include <iostream>
#include <thread>

namespace Engine
{
	namespace Common
	{
		ScopeGaurd::ScopeGaurd(SpinLock& Gaurd) :
			m_Lock(Gaurd)
		{
			m_Lock.Lock();
		}

		ScopeGaurd::~ScopeGaurd(void)
		{
			m_Lock.Release();
		}
	}
}