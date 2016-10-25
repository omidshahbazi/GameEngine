#include "Include\Public\PushSocket.h"

namespace NetworkEngine
{
	PushSocket::PushSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Push)
	{
	}
}