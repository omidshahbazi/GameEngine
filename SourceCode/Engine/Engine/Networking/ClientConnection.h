// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Networking\ConnectionBase.h>

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Networking
	{
		class NETWORKING_API ClientConnection : public ConnectionBase
		{
		public:
			ClientConnection(AllocatorBase *Allocator, const byte *Identifier, uint8 IdentifierLength, float32 Timeout);

			~ClientConnection(void);

			void Connect(const Address &Address);
			
		private:
			Address m_Address;
		};
	}
}

#endif