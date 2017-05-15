// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Networking\Socket.h>

#ifndef CONNECTION_BASE_H
#define CONNECTION_BASE_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ConnectionBase
		{
		public:
			ConnectionBase(AllocatorBase *Allocator, uint32 SendBufferSize, uint32 ReceiveBufferSize);

			~ConnectionBase(void);

		protected:
			bool OpenSocket(void);
			bool BindToPort(uint16 Port);

			virtual void BeginSend();
			virtual bool EndSend(const Address &Address);

			virtual void WriteBuffer(const byte *Buffer, uint32 Length);
			virtual void WriteBuffer(const byte *Buffer, uint32 Index, uint32 Length);

			virtual bool BeginReceive(Address &Address, uint32 &ReceivedLength);
			virtual bool EndReceive(void);

			virtual void ReadBuffer(byte *Buffer, uint32 Length);
			virtual void ReadBuffer(byte *Buffer, uint32 Index, uint32 Length);

			AllocatorBase *GetAllocator(void) const
			{
				return m_Allocator;
			}

		private:
			AllocatorBase *m_Allocator;
			Socket m_Socket;

			byte *m_SendBuffer;
			byte *m_ReceiveBuffer;

			uint32 m_SendBufferLength;
			uint32 m_ReceiveBufferLength;

			uint32 m_SendBufferIndex;
			uint32 m_ReceiveBufferIndex;
		};
	}
}

#endif