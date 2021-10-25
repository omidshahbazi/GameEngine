// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include <Common\PrimitiveTypes.h>

#ifdef WINDOWS
#include <wtypes.h>
#undef CreateWindow
#endif

namespace Engine
{
	namespace EntryPointUtility
	{
#ifdef DEBUG_MODE
#define BEGIN_ENTRY_POINT \
		int32 main(uint8 ArgumentCount, const char8** Arguments) \
		{
#else

#ifdef WINDOWS
#define BEGIN_ENTRY_POINT \
		int32 API_ENTRY wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance, char16* CommandLine, int32 ShowCommandLine) \
		{ \
			int32 ArgumentCount; \
			const char16** Arguments = ConstCast(const char16**, CommandLineToArgvW(GetCommandLineW(), &ArgumentCount));
#endif

#endif

#define END_ENTRY_POINT \
			return 0; \
		}
}

#endif
}