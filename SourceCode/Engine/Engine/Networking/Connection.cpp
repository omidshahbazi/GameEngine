// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\Connection.h>
#include <Networking\ConnectionProtocol.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		Connection::Connection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength) :
			ConnectionBase(Allocator, Identifier, IdentifierLength),
			m_Status(Status::None)
		{
		}

		void Connection::Connect(const Address &Address)
		{
			m_Address = Address;

			OpenSocket();

			SendInternal(m_Address, PACKET_TYPE_HAND_SHAKE_REQUEST);

			m_Status = Status::Connecting;
		}

		bool Connection::Send(const byte *Buffer, uint32 BufferLength)
		{
			return SendInternal(m_Address, PACKET_TYPE_MESSAGE, Buffer, BufferLength);
		}

		bool Connection::Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			static Address address;
			static byte packetType[PACKET_TYPE_SIZE];

			return ReceiveInternal(address, packetType, Buffer, BufferLength, ReceivedLength);
		}

		bool Connection::Listen(uint16 Port)
		{
			m_Address.SetPort(Port);

			if (BindToPort(Port))
			{
				m_Status = Status::Listening;

				return true;
			}

			return false;
		}

		bool Connection::ReceiveInternal(Address &FromAddress, byte *PacketType, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			if (ConnectionBase::ReceiveInternal(FromAddress, PacketType, Buffer, BufferLength, ReceivedLength))
			{
				if (PlatformMemory::AreEqual(PacketType, PACKET_TYPE_HAND_SHAKE_REQUEST, PACKET_TYPE_SIZE))
				{
					SendInternal(FromAddress, PACKET_TYPE_HAND_SHAKE_RESPONSE);

					return false;
				}
				else if (PlatformMemory::AreEqual(PacketType, PACKET_TYPE_HAND_SHAKE_RESPONSE, PACKET_TYPE_SIZE))
				{
					m_Status = Status::Connected;

					m_Address = FromAddress;

					return false;
				}
				else if (PlatformMemory::AreEqual(PacketType, PACKET_TYPE_MESSAGE, PACKET_TYPE_SIZE))
					return true;
			}

			return false;
		}
	}
}