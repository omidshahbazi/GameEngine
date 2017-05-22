// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ReliableConnection.h>
#include <Networking\ConnectionProtocol.h>
#include <Platform\PlatformMemory.h>
#include <Common\BitwiseUtils.h>
#include <Common\Mathematics.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		//send must return message id
		//CheckAck(id) function is a need

		ReliableConnection::ReliableConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize) :
			Connection(Allocator, Identifier, IdentifierLength, SEQUENCE_NUMBER_SIZE + SEQUENCE_NUMBER_SIZE + SendBufferSize, SEQUENCE_NUMBER_SIZE + SEQUENCE_NUMBER_SIZE + ReceiveBufferSize),
			m_SequenceNumebr(1),
			m_RemoteSequenceNumebr(0)
		{
			PlatformMemory::Set(m_Acks, 0, ACKS_COUNT);
		}

		bool ReliableConnection::SendMessage(const byte *Buffer, uint32 BufferLength)
		{
			Connection::BeginSend(PACKET_TYPE_MESSAGE);
			m_SequenceNumebr++;
			WriteValue(m_SequenceNumebr++);

			WriteValue(GetAcks());
			WriteValue(m_RemoteSequenceNumebr);

			WriteBuffer(Buffer, BufferLength);

			return EndSend();
		}

		void ReliableConnection::ReceiveMessage(byte *Buffer, uint32 BufferLength)
		{
			SequenceNumber remoteSeqNum = ReadValue<SequenceNumber>();

			if (!(remoteSeqNum > m_RemoteSequenceNumebr))
				return;

			if (m_RemoteSequenceNumebr != 0)
				SetAcks(m_RemoteSequenceNumebr);

			m_RemoteSequenceNumebr = remoteSeqNum;

			SequenceNumber acks = ReadValue<SequenceNumber>();
			SequenceNumber ack = ReadValue<SequenceNumber>();

			uint8 i = ACKS_COUNT - (ack > ACKS_COUNT ? ACKS_COUNT : ack);
			for (; i < ACKS_COUNT; ++i)
			{
				if (!BitwiseUtils::IsEnabled<SequenceNumber>(acks, i))
				{
?????					int lostNumber = ack - i;
				}
			}

			Connection::ReceiveMessage(Buffer, BufferLength);
		}

		ReliableConnection::SequenceNumber ReliableConnection::GetAcks(void) const
		{
			SequenceNumber acks = 0;

			for (uint8 i = 0; i < ACKS_COUNT; ++i)
				if (m_Acks[i] != 0)
					BitwiseUtils::Enable<SequenceNumber>(acks, i);

			return acks;
		}

		void ReliableConnection::SetAcks(SequenceNumber Number)
		{
			Assert(Number != 0, "Number must be posititve");

			SequenceNumber diff = Number - m_Acks[ACKS_COUNT - 1];
			uint32 validLength = ACKS_COUNT - diff;

			PlatformMemory::Copy(m_Acks, diff, m_Acks, 0, validLength);

			if (diff != 1)
				PlatformMemory::Set(m_Acks + validLength, 0, diff - 1);

			m_Acks[ACKS_COUNT - 1] = Number;
		}
	}
}