// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformOS
		{
		public:
			typedef size_t* Handle;

			struct GUIDData
			{
			public:
				byte Part1[4];
				byte Part2[2];
				byte Part3[2];
				byte Part4[2];
				byte Part5[6];
			};

		public:
			static Handle GetModuleInstance(cwstr ModuleName);

			static Handle GetExecutingModuleInstance(void);

			static void GetExecutablePath(wstr Path);

			static int32 GetErrorCode(void);

			static void GetErrorMessage(str* Message);

			static void GenerateGUID(GUIDData* Data);

			static void GetRoamingPath(wstr Path);

			static bool IsDebuggerAttached(void);
		};
	}
}

#endif