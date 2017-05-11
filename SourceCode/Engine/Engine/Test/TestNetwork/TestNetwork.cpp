
#include <Platform\PlatformNetwork.h>
#include <Platform\PlatformThread.h>

using namespace Engine::Common;
using namespace Engine::Platform;

void GetError()
{
	PlatformNetwork::Errors error = PlatformNetwork::GetLastError();


}

void Receiver()
{
	PlatformNetwork::Handle handle = PlatformNetwork::Create(PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::Types::Datagram, PlatformNetwork::IPProtocols::UDP);


	if (!PlatformNetwork::Bind(handle, PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::InterfaceAddresses::Any, 30001))
	{
		GetError();
		return;
	}

	if (!PlatformNetwork::SetNonBlocking(handle, true))
	{
		GetError();
		return;
	}

	byte buffer[128];
	uint32 receivedLen = 0;
	PlatformNetwork::IP fromIP = 0;
	uint16 fromPort = 0;

	while (true)
	{
		if (!PlatformNetwork::Receive(handle, buffer, sizeof(buffer), receivedLen, fromIP, fromPort, PlatformNetwork::ReceiveModes::None))
		{

		}
	}

	PlatformNetwork::Close(handle);
}

void main()
{
	if (!PlatformNetwork::Initialize())
		return;

	PlatformThread::Begin(Receiver, 512, nullptr);

	PlatformNetwork::Handle handle = PlatformNetwork::Create(PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::Types::Datagram, PlatformNetwork::IPProtocols::UDP);

	const char * buffer = "omid";

	if (!PlatformNetwork::Send(handle, reinterpret_cast<const byte*>(buffer), 5, PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::GetAddress(127, 0, 0, 1), 30001, PlatformNetwork::SendModes::None))
	{
		GetError();
		return;
	}

	PlatformNetwork::Close(handle);

	PlatformNetwork::Shotdown();
}