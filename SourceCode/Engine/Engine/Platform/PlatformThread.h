// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformThread
		{
		public:
			typedef uint32 Handle;
			typedef void(*Procedure)(void*);

		public:
			static Handle Begin(Procedure Procedure, uint32 StackSize, void* Arguments);
			static void End(void);
			static void Close(Handle Thread);
			static void GetDescription(Handle Thread, str Description);
			static void SetDescription(Handle Thread, cstr Description);
			static void Wait(Handle Thread, uint64 Milliseconds);
			static void Join(Handle Thread);
			static void Sleep(uint64 Milliseconds);
			static void SetCoreAffinity(Handle Thread, uint8 CoreIndex);

			static Handle GetCurrentThread(void);

			static uint32 GetID(Handle Thread);
			static uint32 GetID(void);

			static uint8 GetHardwareConcurrency(void);

		public:
			static const uint32 INFINITE_TIME = 0xFFFFFFFF;
		};
	}
}

#endif