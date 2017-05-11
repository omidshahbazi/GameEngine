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
		ClientConnection::ClientConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength) :
			ConnectionBase(Allocator, Identifier, IdentifierLength)
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

		bool ClientConnection::Send(const byte *Buffer, uint32 BufferLength)
		{
			return SendInternal(m_Address, Buffer, BufferLength);
		}

		bool ClientConnection::Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			static Address address;

			return ReceiveInternal(address, Buffer, BufferLength, ReceivedLength);
		}
	}
}