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

	namespace Rendering
	{
		struct ProgramInfo
		{
		public:
			struct DefineInfo
			{
			public:
				String Name;
			};

			typedef Vector<DefineInfo> DefineList;

		public:
			String Source;
			DefineList Defines;
		};
	}
}

#endif