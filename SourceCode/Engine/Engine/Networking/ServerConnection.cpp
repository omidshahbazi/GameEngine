// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ServerConnection.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ServerConnection::ServerConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, float32 Timeout) :
			ConnectionBase(Allocator, Identifier, IdentifierLength, Timeout),
			m_Port(0)
		{
		}

		ServerConnection::~ServerConnection(void)
		{
		}

		void ServerConnection::Listen(uint16 Port)
		{
			m_Port = Port;
			
			BindToPort(m_Port);
		}

		bool ServerConnection::Send(const Address &Address, const byte *Buffer, uint32 BufferLength)
		{
			SendInternal(Address, Buffer, BufferLength);
		}
	}
}