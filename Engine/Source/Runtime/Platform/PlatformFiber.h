// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_FIBER_H
#define PLATFORM_FIBER_H

#include <Common\PrimitiveTypes.h>
#include <functional>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformFiber
		{
		public:
			typedef int64* Handle;
			typedef std::function<void(void* Argument)> Procedure;

		public:
			static Handle Create(Procedure Procedure, uint32 StackSize = 0, void* Arguments = nullptr);
			static void Delete(Handle Fiber);
			static void Switch(Handle Fiber);

			static bool IsRunningOnFiber(void);

			static Handle GetCurrentFiber(void);

			static void* GetData(void);

			static Handle ConvertThreadToFiber(void* Arguments);
		};
	}
}

#endif