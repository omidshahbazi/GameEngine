#include "Include\Public\StreamSocket.h"

namespace NetworkEngine
{
	StreamSocket::StreamSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Stream)
	{
	}
}