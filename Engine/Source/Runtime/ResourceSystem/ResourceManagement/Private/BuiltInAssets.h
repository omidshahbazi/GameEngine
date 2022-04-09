// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_ASSETS_H
#define BUILT_IN_ASSETS_H

#include <Containers\GUID.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceManagement
	{
		namespace Private
		{
			class BuiltInAssets
			{
			public:
				static GUID WHITE_TEXTURE_GUID;
			};
		}
	}
}
#endif