
#include <Platform\PlatformNetwork.h>

using namespace Engine::Common;
using namespace Engine::Platform;

void main()
{
	if (PlatformNetwork::Initialize() != PlatformNetwork::Errors::NoError)
		return;

	PlatformNetwork::Handle handle = PlatformNetwork::Create(PlatformNetwork::AddressFamilies::InterNetwork, PlatformNetwork::Types::Datagram, PlatformNetwork::IPProtocols::UDP);


	if (PlatformNetwork::Bind(handle, PlatformNetwork::AddressFamilies::InterNetwork, 30001) != PlatformNetwork::Errors::NoError)
		return;

	if (PlatformNetwork::SetNonBlocking(handle, true) != PlatformNetwork::Errors::NoError)
		return;

	const char * buffer = "omid";

	if (PlatformNetwork::Send(handle, reinterpret_cast<const byte*>(buffer), 5, PlatformNetwork::AddressFamilies::InterNetwork, 30001, PlatformNetwork::SendModes::None) != PlatformNetwork::Errors::NoError)
		return;

	PlatformNetwork::Close(handle);

	PlatformNetwork::Shotdown();
}