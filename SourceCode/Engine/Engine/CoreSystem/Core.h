// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_H
#define CORE_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace CoreSystem
	{
		class CORESYSTEM_API Core
		{
			SINGLETON_DECLARATION(Core)

		private:
			Core(void);

		};
	}
}

#endif