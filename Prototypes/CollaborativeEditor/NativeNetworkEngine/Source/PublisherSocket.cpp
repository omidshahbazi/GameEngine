#include "Include\Public\PublisherSocket.h"

namespace NetworkEngine
{
	PublisherSocket::PublisherSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Publisher)
	{
	}
}