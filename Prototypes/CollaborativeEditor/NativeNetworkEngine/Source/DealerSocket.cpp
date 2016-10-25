#include "Include\Public\DealerSocket.h"

namespace NetworkEngine
{
	DealerSocket::DealerSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Dealer)
	{
	}
}