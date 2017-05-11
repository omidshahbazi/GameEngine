// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Networking\ConnectionBase.h>

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ClientConnection : public ConnectionBase
		{
		public:
			ClientConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength);

			~ClientConnection(void);

			void Connect(const Address &Address);

			bool Send(const byte *Buffer, uint32 BufferLength);

			bool Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);

		private:
			Address m_Address;
		};
	}
}

#endif