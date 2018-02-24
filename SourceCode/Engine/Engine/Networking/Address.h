// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ADDRESS_H
#define ADDRESS_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformNetwork.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		class NETWORKING_API Address
		{
		public:
			Address(void) :
				m_IP(0),
				m_Port(0)
			{
			}

			Address(uint32 Address, uint16 Port) :
				m_IP(Address),
				m_Port(Port)
			{
			}

			Address(uint8 A, uint8 B, uint8 C, uint8 D, uint16 Port) :
				m_IP(PlatformNetwork::GetIP(A, B, C, D)),
				m_Port(Port)
			{
			}

			bool operator == (const Address &Other) const
			{
				return (m_IP == Other.m_IP && m_Port == Other.m_Port);
			}

			bool operator != (const Address &Other) const
			{
				return !(*this == Other);
			}

			void SetIP(uint32 Address)
			{
				m_IP = Address;
			}

			void SetIP(uint8 A, uint8 B, uint8 C, uint8 D)
			{
				m_IP = PlatformNetwork::GetIP(A, B, C, D);
			}

			void SetPort(uint16 Port)
			{
				m_Port = Port;
			}

			void GetAddress(uint8 &A, uint8 &B, uint8 &C, uint8 &D) const
			{
				PlatformNetwork::GetAddress(m_IP, A, B, C, D);
			}

			PlatformNetwork::IP GetIP(void) const
			{
				return m_IP;
			}

			uint16 GetPort(void) const
			{
				return m_Port;
			}

		private:
			PlatformNetwork::IP m_IP;
			uint16 m_Port;
		};
	}
}

#endif