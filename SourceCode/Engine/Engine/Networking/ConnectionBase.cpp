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
		ConnectionBase::ConnectionBase(AllocatorBase *Allocator, uint32 SendBufferSize, uint32 ReceiveBufferSize) :
			m_Allocator(Allocator),
			m_SendBuffer(nullptr),
			m_ReceiveBuffer(nullptr),
			m_SendBufferLength(SendBufferSize),
			m_ReceiveBufferLength(ReceiveBufferSize),
			m_SendBufferIndex(0),
			m_ReceiveBufferIndex(0)
		{
			Assert(Allocator != nullptr, "Allocator cannot be null");

			if (m_SendBufferLength != 0)
				m_SendBuffer = AllocateMemory(m_Allocator, m_SendBufferLength);
			if (m_ReceiveBufferLength != 0)
				m_ReceiveBuffer = AllocateMemory(m_Allocator, m_ReceiveBufferLength);
		}

		ConnectionBase::~ConnectionBase(void)
		{
			if (m_SendBuffer != nullptr)
				m_Allocator->Deallocate(m_SendBuffer);

			if (m_ReceiveBuffer != nullptr)
				m_Allocator->Deallocate(m_ReceiveBuffer);
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

		void ConnectionBase::BeginSend(void)
		{
			m_SendBufferIndex = 0;
		}

		bool ConnectionBase::EndSend(const Address &Address)
		{
			return m_Socket.Send(Address, m_SendBuffer, m_SendBufferIndex);
		}

		void ConnectionBase::WriteBuffer(const byte *Buffer, uint32 Length)
		{
			WriteBuffer(Buffer, 0, Length);
		}

		void ConnectionBase::WriteBuffer(const byte *Buffer, uint32 Index, uint32 Length)
		{
			Assert(m_SendBufferIndex < m_SendBufferLength - Length, "Out of buffer size");

			PlatformMemory::Copy(Buffer, Index, m_SendBuffer, m_SendBufferIndex, Length);

			m_SendBufferIndex += Length;
		}

		void ConnectionBase::WriteUInt32(uint32 Value)
		{
			Assert(m_SendBufferIndex < m_SendBufferLength - sizeof(Value), "Out of buffer size");

			m_SendBuffer[m_SendBufferIndex++] = (byte)(Value >> 24);
			m_SendBuffer[m_SendBufferIndex++] = (byte)((Value >> 16) & 0xFF);
			m_SendBuffer[m_SendBufferIndex++] = (byte)((Value >> 8) & 0xFF);
			m_SendBuffer[m_SendBufferIndex++] = (byte)(Value & 0xFF);
		}

		bool ConnectionBase::BeginReceive(Address &Address, uint32 &ReceivedLength)
		{
			m_ReceiveBufferIndex = 0;

			return m_Socket.Receive(Address, m_ReceiveBuffer, m_ReceiveBufferLength, ReceivedLength);
		}

		bool ConnectionBase::EndReceive(void)
		{
			return true;
		}

		void ConnectionBase::ReadBuffer(byte *Buffer, uint32 Length)
		{
			ReadBuffer(Buffer, 0, Length);
		}

		void ConnectionBase::ReadBuffer(byte *Buffer, uint32 Index, uint32 Length)
		{
			Assert(m_ReceiveBufferIndex < m_ReceiveBufferLength - Length, "Out of buffer size");

			PlatformMemory::Copy(m_ReceiveBuffer, m_ReceiveBufferIndex, Buffer, Index, Length);

			m_ReceiveBufferIndex += Length;
		}

		uint32 ConnectionBase::ReadUInt32(void)
		{
			return (
				((uint32)m_ReceiveBuffer[m_ReceiveBufferIndex++] << 24) | 
				((uint32)m_ReceiveBuffer[m_ReceiveBufferIndex++] << 16) | 
				((uint32)m_ReceiveBuffer[m_ReceiveBufferIndex++] << 8) | 
				((uint32)m_ReceiveBuffer[m_ReceiveBufferIndex++]));
		}
	}
}