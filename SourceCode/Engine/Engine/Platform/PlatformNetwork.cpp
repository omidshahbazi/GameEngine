// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\PlatformNetwork.h>
#include <Common\BitwiseUtils.h>
#include <winsock2.h>

#pragma comment(lib, "wsock32.lib")

namespace Engine
{
	namespace Platform
	{
		int GetAddressFamiliy(PlatformNetwork::AddressFamilies Family)
		{
			switch (Family)
			{
			case PlatformNetwork::AddressFamilies::InterNetwork:
				return AF_INET;
			}

			return AF_UNSPEC;
		}

		int GetType(PlatformNetwork::Types Type)
		{
			switch (Type)
			{
			case PlatformNetwork::Types::Stream:
				return SOCK_STREAM;

			case PlatformNetwork::Types::Datagram:
				return SOCK_DGRAM;

			case PlatformNetwork::Types::RawProtocol:
				return SOCK_RAW;

			case PlatformNetwork::Types::ReliablyDeliverdMessage:
				return SOCK_RDM;

			case PlatformNetwork::Types::SequenedPacketStream:
				return SOCK_SEQPACKET;
			}

			return 0;
		}

		int GetIPProtocol(PlatformNetwork::IPProtocols IPProtocol)
		{
			switch (IPProtocol)
			{
			case PlatformNetwork::IPProtocols::UDP:
				return IPPROTO_UDP;
			}

			return IPPROTO_MAX;
		}

		int GetSendFlags(PlatformNetwork::SendModes Mode)
		{
			int flags = 0;

			if (BitwiseUtils::IsEnabled(Mode, PlatformNetwork::SendModes::DontRoute))
				flags |= MSG_DONTROUTE;

			if (BitwiseUtils::IsEnabled(Mode, PlatformNetwork::SendModes::OutOfBand))
				flags |= MSG_OOB;

			return IPPROTO_MAX;
		}

		sockaddr_in GetAddressInfo(PlatformNetwork::AddressFamilies AddressFamily, uint16 Port)
		{
			sockaddr_in address;
			address.sin_family = GetAddressFamiliy(AddressFamily);
			address.sin_addr.S_un.S_addr = INADDR_ANY;
			address.sin_port = Port;
			return address;
		}

		bool PlatformNetwork::Initialize(void)
		{
			WSADATA data;
			return (WSAStartup(MAKEWORD(2, 2), &data) == NO_ERROR);
		}

		bool PlatformNetwork::Shotdown(void)
		{
			return (WSACleanup() == NO_ERROR);
		}

		PlatformNetwork::Handle PlatformNetwork::Create(AddressFamilies AddressFamily, Types Type, IPProtocols IPProtocol)
		{
			return (Handle)socket(GetAddressFamiliy(AddressFamily), GetType(Type), GetIPProtocol(IPProtocol));
		}

		void PlatformNetwork::Close(Handle Handle)
		{
			closesocket(Handle);
		}

		bool PlatformNetwork::Bind(Handle Handle, AddressFamilies AddressFamily, uint16 Port)
		{
			sockaddr_in &&address = GetAddressInfo(AddressFamily, Port);

			return (bind(Handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) >= 0);
		}

		bool PlatformNetwork::SetNonBlocking(Handle Handle, bool Enabled)
		{
			DWORD enabled = (Enabled ? 1 : 0);

			return (ioctlsocket(Handle, FIONBIO, &enabled) == NO_ERROR);
		}

		bool PlatformNetwork::Send(Handle Handle, const byte *Buffer, uint32 Length, AddressFamilies AddressFamily, uint16 Port, SendModes Mode)
		{
			sockaddr_in &&address = GetAddressInfo(AddressFamily, Port);
			WSANOTINITIALISED
			return (sendto(Handle, reinterpret_cast<const char*>(Buffer), Length, GetSendFlags(Mode), reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) == Length);
		}
	}
}