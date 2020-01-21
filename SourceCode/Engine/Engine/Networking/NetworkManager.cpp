// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Networking\NetworkManager.h>
#include <Platform\PlatformNetwork.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Platform;

	namespace Networking
	{
		SINGLETON_DEFINITION(NetworkManager)

		NetworkManager::NetworkManager(void)
		{
			Assert(PlatformNetwork::Initialize(), "Network initialization failed");
		}

		NetworkManager::~NetworkManager(void)
		{
			Assert(PlatformNetwork::Shutdown(), "Network shotdown failed");
		}
	}
}