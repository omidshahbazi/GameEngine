// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace Networking
	{
		class NETWORKING_API NetworkManager
		{
			SINGLETON_DECLARATION(NetworkManager)

		public:
			NetworkManager(void);

			~NetworkManager(void);
		};
	}
}

#endif