// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformOS
		{
		public:
			typedef size_t * Handle;

		public:
			static Handle GetModuleInstance(cstr ModuleName);

			static Handle GetExecutingModuleInstance(void);

			static void GetExecutingDirectory(str Directory);

			static int32 GetErrorCode(void);

			static cstr GetErrorMessage(void);
		};
	}
}

#endif