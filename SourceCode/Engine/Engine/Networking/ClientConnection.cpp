// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ClientConnection.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ClientConnection::ClientConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, float32 Timeout) :
			ConnectionBase(Allocator, Identifier, IdentifierLength, Timeout)
		{
		}

		ClientConnection::~ClientConnection(void)
		{
		}

		void ClientConnection::Connect(const Address &Address)
		{
			m_Address = Address;

			OpenSocket();
		}
	}
}