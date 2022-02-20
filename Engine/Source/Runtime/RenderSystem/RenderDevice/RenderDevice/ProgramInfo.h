// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_INFO_H
#define PROGRAM_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace RenderDevice
	{
		struct ProgramInfo
		{
		public:
			typedef Vector<String> DefineList;

		public:
			String Source;
			DefineList Defines;
			bool DebugMode;
		};
	}
}

#endif