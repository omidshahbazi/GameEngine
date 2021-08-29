// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SCOPE_GAURD_H
#define SCOPE_GAURD_H

#include <Common\SpinLock.h>

namespace Engine
{
	namespace Common
	{
		class COMMON_API ScopeGaurd
		{
		public:
			ScopeGaurd(SpinLock& Lock);
			~ScopeGaurd(void);

			ScopeGaurd(const SpinLock&) = delete;
			ScopeGaurd& operator=(const SpinLock&) = delete;

		private:
			SpinLock& m_Lock;
		};
	}
}

#endif