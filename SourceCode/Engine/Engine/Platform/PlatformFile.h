// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformFile
		{
		public:
			typedef uint32 Handle;
			typedef void(*Procedure)(void);

		public:
			static Handle Begin(Procedure Procedure, uint32 StackSize, void *Arguments);
			static void End(void);
			static void Wait(Handle Thread, uint64 Milliseconds);
			static void Join(void);
			static void Sleep(uint64 Milliseconds);
			static void SetCoreAffinity(Handle Thread, uint8 CoreIndex);

			static uint8 GetHardwareConcurrency(void);

		public:
			static const uint32 INFINITE_TIME = 0xFFFFFFFF;
		};
	}
}

#endif