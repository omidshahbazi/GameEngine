// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RELIABLE_CONNECTION_H
#define RELIABLE_CONNECTION_H

#include <Networking\Connection.h>

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ReliableConnection : public Connection
		{
		public:
			enum class MessageStatus
			{
				Unknown,
				Delivered,
				Lost
			};

			typedef uint16 SequenceNumber;

		private:
			static const uint8 SEQUENCE_NUMBER_SIZE = sizeof(SequenceNumber);
			static const uint8 ACKS_COUNT = SEQUENCE_NUMBER_SIZE * 8;

		public:
			ReliableConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize);

			virtual bool Send(const byte *Buffer, uint32 BufferLength, SequenceNumber &MessageNumber);

			virtual MessageStatus GetMessageStatus(SequenceNumber MessageNumber);

		protected:
			virtual bool Send(const byte *Buffer, uint32 BufferLength) override
			{
				return Connection::Send(Buffer, BufferLength);
			}

			virtual bool SendMessage(const byte *Buffer, uint32 BufferLength) override;
			virtual	void ReceiveMessage(byte *Buffer, uint32 BufferLength) override;

		private:
			SequenceNumber GetAcks(void) const;
			void SetAcks(SequenceNumber Number);

		private:
			SequenceNumber m_SequenceNumebr;

			SequenceNumber m_RemoteSequenceNumebr;
			SequenceNumber m_Acks[ACKS_COUNT];

			SequenceNumber m_RemoteAck;
			SequenceNumber m_RemoteAcks;
		};
	}
}

#endif