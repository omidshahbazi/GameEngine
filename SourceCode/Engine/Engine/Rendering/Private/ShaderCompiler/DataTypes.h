// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				enum class DataTypes
				{
					Void = 0,
					Float,
					Float2,
					Float3,
					Float4,
					Matrix4,
					Unknown
				};
			}
		}
	}
}

#endif