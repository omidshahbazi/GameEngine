#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API RequestSocket : public SocketBase
	{
	public:
		RequestSocket(Context *Context, int BufferSize);
	};
}

#endif