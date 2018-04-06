// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API FileSystem
		{
		public:
			static String GetExecutingPath(void);
		};
	}
}

#endif