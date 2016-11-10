// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef MULTI_THREADING_H
#define MULTI_THREADING_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class MultiThreading
		{
		public:
			static uint32 GetHardwareConcurrency(void);
		};
	}
}

#endif