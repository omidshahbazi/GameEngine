// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include <Networking\Connection.h>

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ServerConnection : public Connection
		{
		public:
			ServerConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength);

			~ServerConnection(void);

			virtual bool Send(const Address &Address, const byte *Buffer, uint32 BufferLength);

			virtual bool Receive(Address &Address, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);
		};
	}
}

#endif