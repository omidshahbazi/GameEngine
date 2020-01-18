#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API PullSocket : public SocketBase
	{
	public:
		PullSocket(Context *Context, int BufferSize);
	};
}

#endif