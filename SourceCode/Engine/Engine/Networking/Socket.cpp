// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Networking\Socket.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Networking
	{
		Socket::Socket(void) :
			m_Handle(0)
		{

		}

		Socket::~Socket(void)
		{
			Close();
		}

		bool Socket::Open()
		{
			Assert(!IsOpen(), "Socket already open");

			m_Handle = PlatformNetwork::Create(PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::Types::Datagram, PlatformNetwork::IPProtocols::UDP);

			return IsOpen();
		}

		bool Socket::Bind(uint16 Port)
		{
			Assert(IsOpen(), "Socket is not open");

			return PlatformNetwork::Bind(m_Handle, PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::InterfaceAddresses::Any, Port);
		}

		bool Socket::SetNonBlocking(bool Enabled)
		{
			return PlatformNetwork::SetNonBlocking(m_Handle, Enabled);
		}

		void Socket::Close(void)
		{
			if (m_Handle == 0)
				return;

			PlatformNetwork::Close(m_Handle);

			m_Handle = 0;
		}

		bool Socket::Send(const Address &DestinationAddress, const byte *Buffer, uint32 Length)
		{
			Assert(IsOpen(), "Socket is not open");
			Assert(Buffer != nullptr, "Buffer cannot be null");
			Assert(Length != 0, "Length must be positive");

			return PlatformNetwork::Send(m_Handle, Buffer, Length, PlatformNetwork::AddressFamilies::InterNetwork, DestinationAddress.GetIP(), DestinationAddress.GetPort(), PlatformNetwork::SendModes::None);
		}

		bool Socket::Receive(Address &SenderAddress, byte *Buffer, uint32 BufferLength, uint32 &ReceivedLength)
		{
			Assert(IsOpen(), "Socket is not open");
			Assert(Buffer != nullptr, "Buffer cannot be null");
			Assert(BufferLength != 0, "BufferLength must be positive");

			uint32 address = 0;
			uint16 port = 0;

			if (PlatformNetwork::Receive(m_Handle, Buffer, BufferLength, ReceivedLength, address, port, PlatformNetwork::ReceiveModes::None))
			{
				SenderAddress.SetIP(address);
				SenderAddress.SetPort(port);

				return true;
			}

			return false;
		}
	}
}