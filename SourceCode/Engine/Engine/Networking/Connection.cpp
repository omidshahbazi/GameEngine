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
		Connection::Connection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, uint32 SendBufferSize, uint32 ReceiveBufferSize) :
			ConnectionBase(Allocator, IdentifierLength + PACKET_TYPE_SIZE + SendBufferSize, IdentifierLength + PACKET_TYPE_SIZE + ReceiveBufferSize),
			m_Identifier(nullptr),
			m_IdentifierLength(IdentifierLength),
			m_Status(Status::None),
			m_SequenceNumebr(0)
		{
			Assert(Identifier != nullptr, "Identifier cannot be null");
			Assert(IdentifierLength != 0, "IdentifierLength must be positive");

			m_Identifier = AllocateMemory(GetAllocator(), IdentifierLength);
			PlatformMemory::Copy(Identifier, m_Identifier, m_IdentifierLength);
		}

		Connection::~Connection(void)
		{
			GetAllocator()->Deallocate(m_Identifier);
		}

		void Connection::Connect(const Address &Address)
		{
			Assert(m_Status == Status::None, "Connection already connected");

			m_Address = Address;

			OpenSocket();

			SendConnectRequest();

			m_Status = Status::Connecting;
		}

		bool Connection::Send(const byte *Buffer, uint32 BufferLength)
		{
			return SendMessage(Buffer, BufferLength);
		}

		bool Connection::Receive(byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			Assert(Buffer != nullptr, "Buffer cannot be null");
			Assert(BufferLength != 0, "BufferLength must be positive");

			static Address address;
			uint32 receivedLength = 0;

			bool result = false;

			uint32 headerSize = m_IdentifierLength + PACKET_TYPE_SIZE;

			byte *headerBuffer = AllocateMemory(GetAllocator(), headerSize);

			if (BeginReceive(address, receivedLength) && receivedLength >= headerSize)
			{
				ReadBuffer(headerBuffer, headerSize);

				if (PlatformMemory::AreEqual(headerBuffer, m_Identifier, m_IdentifierLength))
				{
					if (PlatformMemory::AreEqual(headerBuffer, m_IdentifierLength, PACKET_TYPE_HAND_SHAKE_REQUEST, 0, PACKET_TYPE_SIZE))
					{
						m_Address = address;

						SendConnectResponse();
					}
					else if (PlatformMemory::AreEqual(headerBuffer, m_IdentifierLength, PACKET_TYPE_HAND_SHAKE_RESPONSE, 0, PACKET_TYPE_SIZE))
					{
						m_Status = Status::Connected;

						m_Address = address;
					}
					else if (PlatformMemory::AreEqual(headerBuffer, m_IdentifierLength, PACKET_TYPE_MESSAGE, 0, PACKET_TYPE_SIZE))
					{
						ReceivedLength = receivedLength - headerSize;

						ReadBuffer(Buffer, ReceivedLength);

						result = true;
					}
				}
			}

			GetAllocator()->Deallocate(headerBuffer);

			EndReceive();

			return result;
		}

		bool Connection::Listen(uint16 Port)
		{
			Assert(m_Status == Status::None, "Connection already connected");

			m_Address.SetPort(Port);

			if (BindToPort(Port))
			{
				m_Status = Status::Listening;

				return true;
			}

			return false;
		}

		//bool Connection::ReceiveInternal(Address &Address, byte *PacketType)
		//{
		//	++m_SequenceNumebr;


		//}

		bool Connection::EndSend(void)
		{
			return ConnectionBase::EndSend(m_Address);
		}

		bool Connection::SendConnectRequest(void)
		{
			BeginSend();

			WriteBuffer(m_Identifier, m_IdentifierLength);
			WriteBuffer(PACKET_TYPE_HAND_SHAKE_REQUEST, PACKET_TYPE_SIZE);

			return EndSend();
		}

		bool Connection::SendConnectResponse(void)
		{
			BeginSend();

			WriteBuffer(m_Identifier, m_IdentifierLength);
			WriteBuffer(PACKET_TYPE_HAND_SHAKE_RESPONSE, PACKET_TYPE_SIZE);

			return EndSend();
		}

		bool Connection::SendMessage(const byte *Buffer, uint32 BufferLength)
		{
			BeginSend();

			WriteBuffer(m_Identifier, m_IdentifierLength);
			WriteBuffer(PACKET_TYPE_MESSAGE, PACKET_TYPE_SIZE);
			WriteBuffer(Buffer, BufferLength);

			return EndSend();
		}
	}
}