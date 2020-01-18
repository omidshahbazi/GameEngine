#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API StreamSocket : public SocketBase
	{
	public:
		StreamSocket(Context *Context, int BufferSize);
	};
}

#endif