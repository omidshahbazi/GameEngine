
#include <Platform\PlatformNetwork.h>

using namespace Engine::Platform;

void main()
{
	if (!PlatformNetwork::Initialize())
		return;

	PlatformNetwork::Handle handle = PlatformNetwork::Create(PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::Types::Datagram, PlatformNetwork::IPProtocols::UDP);


	if (!PlatformNetwork::Bind(handle, PlatformNetwork::AddressFamilies::InterNetwork, 30001))
		return;

	PlatformNetwork::Close(handle);

	PlatformNetwork::Shotdown();
}