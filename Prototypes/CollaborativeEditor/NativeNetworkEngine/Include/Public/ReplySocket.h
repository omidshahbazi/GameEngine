#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API ReplySocket : public SocketBase
	{
	public:
		ReplySocket(Context *Context, int BufferSize);
	};
}

#endif