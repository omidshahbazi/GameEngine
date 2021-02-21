// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_DATA_TYPE_H
#define PROGRAM_DATA_TYPE_H

namespace Engine
{
	namespace Rendering
	{
		enum class ProgramDataTypes
		{
			Void = 0,
			Bool,
			Float,
			Double,
			Float2,
			Double2,
			Float3,
			Double3,
			Float4,
			Double4,
			Matrix4,
			Texture2D,
			Unknown
		};
	}
}

#endif