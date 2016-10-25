#include "Include\Public\ReplySocket.h"

namespace NetworkEngine
{
	ReplySocket::ReplySocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Reply)
	{
	}
}