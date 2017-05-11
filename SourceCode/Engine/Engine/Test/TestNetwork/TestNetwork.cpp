
#include <Platform\PlatformNetwork.h>
#include <Networking\Socket.h>
#include <Threading\Thread.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::Networking;
using namespace Engine::Platform;

void GetError()
{
	PlatformNetwork::Errors error = PlatformNetwork::GetLastError();


}

void Receiver()
{
	Socket socket;

	if (!socket.Open())
	{
		GetError();
		return;
	}

	if (!socket.Bind(30001))
	{
		GetError();
		return;
	}

	if (!socket.SetNonBlocking(true))
	{
		GetError();
		return;
	}

	byte buffer[128];
	uint32 receivedLen = 0;
	Address senderAddress;

	while (true)
	{
		if (!socket.Receive(senderAddress, buffer, sizeof(buffer), receivedLen))
		{
			GetError();
			continue;
		}

		if (receivedLen != 0)
		{
			std::cout << buffer;

			uint8 a, b, c, d;
			senderAddress.GetAddress(a, b, c, d);

			std::cout << " received from " << (int16)a << "." << (int16)b << "." << (int16)c << "." << (int16)d << ":" << senderAddress.GetPort() << "\n";
		}
	}
}

void Sender()
{
	Socket socket;

	if (!socket.Open())
	{
		GetError();
		return;
	}

	if (!socket.SetNonBlocking(true))
	{
		GetError();
		return;
	}

	const char * buffer = "omid";
	Address destAddress(127, 0, 0, 1, 30001);

	while (true)
	{
		if (!socket.Send(destAddress, reinterpret_cast<const byte*>(buffer), 5))
		{
			GetError();
			return;
		}
	}
}

void main()
{
	if (!PlatformNetwork::Initialize())
		return;

	Engine::Threading::Thread recvThread;
	recvThread.Initialize(Receiver, 512, nullptr);

	Engine::Threading::Thread sendThread;
	sendThread.Initialize(Sender, 512, nullptr);

	system("pause");

	PlatformNetwork::Shotdown();
}