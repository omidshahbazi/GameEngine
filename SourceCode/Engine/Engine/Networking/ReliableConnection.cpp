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
			m_RemoteSequenceNumebr(1)
		{
			PlatformMemory::Set(reinterpret_cast<byte*>(m_Acks), 0, sizeof(m_Acks));
		}

		bool ReliableConnection::SendMessage(const byte *Buffer, uint32 BufferLength)
		{
			Connection::BeginSend(PACKET_TYPE_MESSAGE);

			WriteUInt32(m_SequenceNumebr++);
			WriteUInt32(GetAcks());

			WriteBuffer(Buffer, BufferLength);

			return EndSend();
		}

		void ReliableConnection::ReceiveMessage(byte *Buffer, uint32 BufferLength)
		{
			uint32 remoteSeqNum = ReadUInt32();

			if (remoteSeqNum > m_RemoteSequenceNumebr)
			{
				m_RemoteSequenceNumebr = remoteSeqNum;

				SetAcks(remoteSeqNum);
			}

			uint32 acks = ReadUInt32();

			Connection::ReceiveMessage(Buffer, BufferLength);
		}

		uint32 ReliableConnection::GetAcks(void) const
		{
			uint32 acks = 0;

			for (int i = 0; i < ACKS_COUNT; ++i)
				acks |= 1 << i;

			return acks;
		}

		void ReliableConnection::SetAcks(uint32 Number)
		{
			if (!(Number > ACKS_COUNT))
			{
				m_Acks[Number - 1] = Number;
				return;
			}

			uint32 diff = Number - ACKS_COUNT;

			PlatformMemory::Copy(m_Acks, diff, m_Acks, 0, ACKS_COUNT - diff);

			m_Acks[ACKS_COUNT - 1] = Number;
		}
	}
}