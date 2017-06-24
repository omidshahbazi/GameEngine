// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef PLATFORM_FIBER_H
#define PLATFORM_FIBER_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		//FlsAlloc
		//FlsFree
		//FlsGetValue
		//FlsSetValue

		class PLATFORM_API PlatformFiber
		{
		public:
			typedef int64 * Handle;
			typedef void(*Procedure)(void*);

		public:
			static Handle Create(Procedure Procedure, uint32 StackSize, void *Arguments);
			static void Delete(Handle Fiber);
			static void Switch(Handle Fiber);

			static void *GetData(void);

			static Handle ConvertThreadToFiber(void *Arguments);
		};
	}
}

#endif