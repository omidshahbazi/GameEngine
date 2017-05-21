// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ReliableConnection.h>
#include <Networking\ConnectionProtocol.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ReliableConnection::ReliableConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize) :
			Connection(Allocator, Identifier, IdentifierLength, PACKET_SEQUENCE_NUMBER_SIZE + PACKET_ACKS_SIZE + SendBufferSize, PACKET_SEQUENCE_NUMBER_SIZE + PACKET_ACKS_SIZE + ReceiveBufferSize),
			m_SequenceNumebr(1),
			m_RemoteSequenceNumebr(0)
		{
			PlatformMemory::Set(m_Acks, 0, ACKS_COUNT);
		}
		???
		send must return message id
		CheckAck(id) function is a need
		???
		bool ReliableConnection::SendMessage(const byte *Buffer, uint32 BufferLength)
		{
			Connection::BeginSend(PACKET_TYPE_MESSAGE);

			WriteUInt32(m_SequenceNumebr++);

			WriteUInt32(GetAcks());
			WriteUInt32(m_RemoteSequenceNumebr);

			WriteBuffer(Buffer, BufferLength);

			return EndSend();
		}

		void ReliableConnection::ReceiveMessage(byte *Buffer, uint32 BufferLength)
		{
			uint32 remoteSeqNum = ReadUInt32();

			if (!(remoteSeqNum > m_RemoteSequenceNumebr))
				return;

			if (m_RemoteSequenceNumebr != 0)
				SetAcks(m_RemoteSequenceNumebr);

			m_RemoteSequenceNumebr = remoteSeqNum;

			uint32 acks = ReadUInt32();
			uint32 ack = ReadUInt32();

			Connection::ReceiveMessage(Buffer, BufferLength);
		}

		uint32 ReliableConnection::GetAcks(void) const
		{
			uint32 acks = 0;

			for (uint32 i = 0; i < ACKS_COUNT; ++i)
				if (m_Acks[i] != 0)
					acks |= 1 << i;

			return acks;
		}

		void ReliableConnection::SetAcks(uint32 Number)
		{
			Assert(Number != 0, "Number must be posititve");

			uint32 diff = Number - m_Acks[ACKS_COUNT - 1];
			uint32 validLength = ACKS_COUNT - diff;

			PlatformMemory::Copy(m_Acks, diff, m_Acks, 0, validLength);

			if (diff != 1)
				PlatformMemory::Set(m_Acks + validLength, 0, diff - 1);

			m_Acks[ACKS_COUNT - 1] = Number;
		}
	}
}