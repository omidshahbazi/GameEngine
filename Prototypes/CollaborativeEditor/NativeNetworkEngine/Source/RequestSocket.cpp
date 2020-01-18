#include "Include\Public\RequestSocket.h"

namespace NetworkEngine
{
	RequestSocket::RequestSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Request)
	{
	}
}