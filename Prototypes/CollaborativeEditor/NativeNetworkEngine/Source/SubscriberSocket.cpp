#include "Include\Public\SubscriberSocket.h"

namespace NetworkEngine
{
	SubscriberSocket::SubscriberSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Subscriber)
	{
	}
}