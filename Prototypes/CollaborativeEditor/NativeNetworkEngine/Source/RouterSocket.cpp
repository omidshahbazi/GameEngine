#include "Include\Public\RouterSocket.h"

namespace NetworkEngine
{
	RouterSocket::RouterSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Router)
	{
	}
}