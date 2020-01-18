#include "Include\Public\PullSocket.h"

namespace NetworkEngine
{
	PullSocket::PullSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Pull)
	{
	}
}