// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_COMMON_H
#define RENDERING_COMMON_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace RenderCommon
	{
#if X64
		typedef uint64 ResourceHandle;
#else
		typedef uint32 ResourceHandle;
#endif

		typedef uint32 ProgramConstantHandle;
		typedef int32 ProgramConstantHash;
	}
}

#endif