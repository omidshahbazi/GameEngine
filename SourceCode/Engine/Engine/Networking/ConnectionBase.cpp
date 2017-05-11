// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Networking\ConnectionBase.h>
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

		bool ConnectionBase::SendInternal(const Address &Address, const byte *Buffer, uint32 BufferLength)
		{
			uint32 bufferSize = m_IdentifierLength + BufferLength;
			byte *buffer = AllocateMemory(m_Allocator, bufferSize);

			PlatformMemory::Copy(m_Identifier, buffer, m_IdentifierLength);
			PlatformMemory::Copy(Buffer, 0, buffer, m_IdentifierLength, BufferLength);

			bool result = m_Socket.Send(Address, buffer, bufferSize);

			m_Allocator->Deallocate(buffer);

			return result;
		}

		bool ConnectionBase::ReceiveInternal(Address &Address, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			uint32 bufferSize = m_IdentifierLength + BufferLength;
			byte *buffer = AllocateMemory(m_Allocator, bufferSize);

			bool result = true;

			if (!m_Socket.Receive(Address, buffer, bufferSize, ReceivedLength))
			{
				ReceivedLength = 0;
				result = false;
				goto CleanUp;
			}

			if (ReceivedLength < m_IdentifierLength)
			{
				ReceivedLength = 0;
				result = false;
				goto CleanUp;
			}

			for (uint8 i = 0; i < m_IdentifierLength; ++i)
				if (buffer[i] != m_Identifier[i])
				{
					ReceivedLength = 0;
					result = false;
					goto CleanUp;
				}

			PlatformMemory::Copy(buffer, m_IdentifierLength, Buffer, 0, ReceivedLength - m_IdentifierLength);

		CleanUp:
			m_Allocator->Deallocate(buffer);

			return result;
		}
	}
}