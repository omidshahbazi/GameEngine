// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_COMMON_H
#define RENDERING_COMMON_H

namespace Engine
{
	namespace Rendering
	{
		enum class RenderQueues
		{
			Default = 0,
			Geometry,
			Lighting,
			Transparent,
			COUNT
		};
	}
}

#endif