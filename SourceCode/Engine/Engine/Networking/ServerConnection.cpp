// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ServerConnection.h>
#include <Networking\ConnectionProtocol.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ServerConnection::ServerConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength) :
			Connection(Allocator, Identifier, IdentifierLength)
		{
		}

		ServerConnection::~ServerConnection(void)
		{
		}

		bool ServerConnection::Send(const Address &Address, const byte *Buffer, uint32 BufferLength)
		{
			return SendInternal(Address, PACKET_TYPE_MESSAGE, Buffer, BufferLength);
		}

		bool ServerConnection::Receive(Address &Address, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			static byte packetType[PACKET_TYPE_SIZE];

			return ReceiveInternal(Address, packetType, Buffer, BufferLength, ReceivedLength);
		}
	}
}