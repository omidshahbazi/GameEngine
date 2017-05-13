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
		class NETWORKING_API Connection : public ConnectionBase
		{
		public:
			enum class Status
			{
				None,
				Listening,
				Connecting,
				Connected,
				Disconnected
			};

		public:
			Connection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength);

			virtual void Connect(const Address &Address);

			virtual bool Send(const byte *Buffer, uint32 BufferLength);

			virtual bool Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);

			virtual bool Listen(uint16 Port);

			Status GetStatus(void) const
			{
				return m_Status;
			}

		protected:
			virtual bool ReceiveInternal(Address &FromAddress, byte *PacketType, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength) override;

		private:
			Address m_Address;
			Status m_Status;
		};
	}
}

#endif