// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include <Networking\ConnectionBase.h>

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
			Connection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize);

			~Connection(void);

			virtual void Connect(const Address &Address);

			virtual bool Send(const byte *Buffer, uint32 BufferLength);

			virtual bool Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength);

			virtual bool Listen(uint16 Port);

			Status GetStatus(void) const
			{
				return m_Status;
			}

		protected:
			virtual void BeginSend(const byte *RequestType);
			virtual bool EndSend(void);

			virtual bool SendConnectRequest(void);
			virtual bool SendConnectResponse(void);

			virtual bool SendMessage(const byte *Buffer, uint32 BufferLength);

			virtual void ReceiveMessage(byte *Buffer, uint32 BufferLength);

		private:
			byte *m_Identifier;
			uint8 m_IdentifierLength;

			Address m_Address;

			Status m_Status;
		};
	}
}

#endif