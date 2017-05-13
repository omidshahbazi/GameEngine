// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

namespace Engine
{
	namespace Networking
	{
		class NETWORKING_API NetworkManager
		{
		public:
			NetworkManager(void);

			~NetworkManager(void);

			static NetworkManager &GetInstance(void);

		private:
			static NetworkManager *instance;
		};
	}
}

#endif