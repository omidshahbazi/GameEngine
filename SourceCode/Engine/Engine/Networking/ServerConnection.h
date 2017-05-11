// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Networking\ConnectionBase.h>

#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ServerConnection : public ConnectionBase
		{
		public:
			ServerConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength);

			~ServerConnection(void);

			void Listen(uint16 Port);

			bool Send(const Address &Address, const byte *Buffer, uint32 BufferLength);

			bool Receive(Address &Address, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);

		private:
			uint16 m_Port;
		};
	}
}

#endif