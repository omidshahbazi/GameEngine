// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ReliableConnection.h>
#include <Networking\ConnectionProtocol.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ReliableConnection::ReliableConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize) :
			Connection(Allocator, Identifier, IdentifierLength, PACKET_SEQUENCE_NUMBER_SIZE + PACKET_ACKS_SIZE + SendBufferSize, PACKET_SEQUENCE_NUMBER_SIZE + PACKET_ACKS_SIZE + ReceiveBufferSize),
			m_SequenceNumebr(0)
		{
		}

		bool ReliableConnection::SendMessage(const byte *Buffer, uint32 BufferLength)
		{
			Connection::BeginSend(PACKET_TYPE_MESSAGE);

			WriteUInt32(m_SequenceNumebr++);

			WriteBuffer(Buffer, BufferLength);

			return EndSend();
		}

		void ReliableConnection::ReceiveMessage(byte *Buffer, uint32 BufferLength)
		{
			m_RemoteSequenceNumebr = ReadUInt32();

			Connection::ReceiveMessage(Buffer, BufferLength);
		}
	}
}