#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API DealerSocket : public SocketBase
	{
	public:
		DealerSocket(Context *Context, int BufferSize);
	};
}

#endif