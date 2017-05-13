// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\NetworkManager.h>
#include <Platform\PlatformNetwork.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Platform;

	namespace Networking
	{
		NetworkManager *NetworkManager::instance = nullptr;

		NetworkManager::NetworkManager(void)
		{
			Assert(PlatformNetwork::Initialize(), "Network initialization failed");
		}

		NetworkManager::~NetworkManager(void)
		{
			Assert(PlatformNetwork::Shutdown(), "Network shotdown failed");
		}

		NetworkManager &NetworkManager::GetInstance(void)
		{
			if (instance == nullptr)
				instance = new NetworkManager;

			return *instance;
		}
	}
}