// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

		private:
			NetworkManager(void);

			~NetworkManager(void);
		};
	}
}

#endif