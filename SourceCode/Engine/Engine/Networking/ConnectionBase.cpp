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
		ConnectionBase::ConnectionBase(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, float32 Timeout) :
			m_Allocator(Allocator),
			m_Identifier(nullptr),
			m_IdentifierLength(IdentifierLength),
			m_Timeout(Timeout)
		{
			Assert(Allocator != nullptr, "Allocator cannot be null");
			Assert(Identifier != nullptr, "Identifier cannot be null");
			Assert(IdentifierLength != 0, "IdentifierLength must be positive");
			Assert(Timeout != 0.0F, "Timeout must be positive");

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
			m_Socket.SetNonBlocking(true);
		}

		bool ConnectionBase::BindToPort(uint16 Port)
		{
			m_Socket.Open();
			m_Socket.SetNonBlocking(true);
			m_Socket.Bind(Port);
		}

		bool ConnectionBase::SendInternal(const Address &Address, const byte *Buffer, uint32 BufferLength)
		{
			uint32 bufferSize = m_IdentifierLength + BufferLength;
			byte *buffer = AllocateMemory(m_Allocator, bufferSize);

			PlatformMemory::Copy(m_Identifier, buffer, m_IdentifierLength);
			PlatformMemory::Copy(Buffer, 0, buffer, m_IdentifierLength, BufferLength);

			m_Socket.Send(Address, buffer, bufferSize);

			m_Allocator->Deallocate(buffer);
		}
	}
}