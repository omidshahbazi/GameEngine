// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_INFO_H
#define SHADER_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		struct ShaderInfo
		{
		public:
			struct DefineInfo
			{
			public:
				String Name;
				int32 Value;
			};

			typedef Vector<DefineInfo> DefineList;

		public:
			String Source;
			DefineList Defines;
		};
	}
}

#endif