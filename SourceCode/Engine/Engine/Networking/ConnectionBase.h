// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Networking\Socket.h>

#ifndef CONNECTION_BASE_H
#define CONNECTION_BASE_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ConnectionBase
		{
		public:
			ConnectionBase(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength);

			~ConnectionBase(void);

		protected:
			bool OpenSocket(void);
			bool BindToPort(uint16 Port);

			virtual bool SendInternal(const Address &Address, const byte *PacketType);
			virtual bool SendInternal(const Address &Address, const byte *PacketType, const byte *Buffer, uint32 BufferLength);

			virtual bool ReceiveInternal(Address &Address, byte *PacketType);
			virtual bool ReceiveInternal(Address &Address, byte *PacketType, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);

		private:
			AllocatorBase *m_Allocator;
			byte *m_Identifier;
			uint8 m_IdentifierLength;
			Socket m_Socket;
		};
	}
}

#endif