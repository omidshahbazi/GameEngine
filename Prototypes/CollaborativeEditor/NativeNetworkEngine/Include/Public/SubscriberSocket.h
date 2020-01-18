#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API SubscriberSocket : public SocketBase
	{
	public:
		SubscriberSocket(Context *Context, int BufferSize);
	};
}

#endif