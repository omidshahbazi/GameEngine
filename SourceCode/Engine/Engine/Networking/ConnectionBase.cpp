// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ConnectionBase.h>
#include <Networking\ConnectionProtocol.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		ConnectionBase::ConnectionBase(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength) :
			m_Allocator(Allocator),
			m_Identifier(nullptr),
			m_IdentifierLength(IdentifierLength)
		{
			Assert(Allocator != nullptr, "Allocator cannot be null");
			Assert(Identifier != nullptr, "Identifier cannot be null");
			Assert(IdentifierLength != 0, "IdentifierLength must be positive");

			m_Identifier = AllocateMemory(m_Allocator, IdentifierLength);
			PlatformMemory::Copy(Identifier, m_Identifier, m_IdentifierLength);
		}

		ConnectionBase::~ConnectionBase(void)
		{
			m_Allocator->Deallocate(m_Identifier);
		}

		bool ConnectionBase::OpenSocket(void)
		{
			m_Socket.Open();
			return m_Socket.SetNonBlocking(true);
		}

		bool ConnectionBase::BindToPort(uint16 Port)
		{
			m_Socket.Open();
			m_Socket.SetNonBlocking(true);
			return m_Socket.Bind(Port);
		}

		bool ConnectionBase::SendInternal(const Address &Address, const byte *PacketType)
		{
			return SendInternal(Address, PacketType, nullptr, 0);
		}

		bool ConnectionBase::SendInternal(const Address &Address, const byte *PacketType, const byte *Buffer, uint32 BufferLength)
		{
			Assert(PacketType != nullptr, "PacketType cannot be null");

			uint32 bufferSize = m_IdentifierLength + PACKET_TYPE_SIZE + BufferLength;
			byte *buffer = AllocateMemory(m_Allocator, bufferSize);

			uint32 index = 0;
			PlatformMemory::Copy(m_Identifier, buffer, m_IdentifierLength);

			index += m_IdentifierLength;
			PlatformMemory::Copy(PacketType, 0, buffer, index, PACKET_TYPE_SIZE);

			if (Buffer != nullptr)
			{
				index += PACKET_TYPE_SIZE;
				PlatformMemory::Copy(Buffer, 0, buffer, index, BufferLength);
			}

			bool result = m_Socket.Send(Address, buffer, bufferSize);

			m_Allocator->Deallocate(buffer);

			return result;
		}

		bool ConnectionBase::ReceiveInternal(Address &Address, byte *PacketType)
		{
			uint32 unused = 0;
			return ReceiveInternal(Address, PacketType, nullptr, 0, unused);
		}

		bool ConnectionBase::ReceiveInternal(Address &Address, byte *PacketType, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			Assert(PacketType != nullptr, "PacketType cannot be null");

			uint32 headerSize = m_IdentifierLength + PACKET_TYPE_SIZE;
			uint32 bufferSize = headerSize + BufferLength;
			byte *buffer = AllocateMemory(m_Allocator, bufferSize);

			bool result = true;

			if (!m_Socket.Receive(Address, buffer, bufferSize, ReceivedLength))
			{
				ReceivedLength = 0;
				result = false;
				goto CleanUp;
			}

			if (ReceivedLength < headerSize)
			{
				ReceivedLength = 0;
				result = false;
				goto CleanUp;
			}

			if (!PlatformMemory::AreEqual(buffer, m_Identifier, m_IdentifierLength))
			{
				ReceivedLength = 0;
				result = false;
				goto CleanUp;
			}

			PlatformMemory::Copy(buffer, m_IdentifierLength, PacketType, 0, PACKET_TYPE_SIZE);

			ReceivedLength -= headerSize;

			if (Buffer != nullptr && ReceivedLength != 0)
				PlatformMemory::Copy(buffer, headerSize, Buffer, 0, ReceivedLength);

		CleanUp:
			m_Allocator->Deallocate(buffer);

			return result;
		}
	}
}