#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API PublisherSocket : public SocketBase
	{
	public:
		PublisherSocket(Context *Context, int BufferSize);
	};
}

#endif