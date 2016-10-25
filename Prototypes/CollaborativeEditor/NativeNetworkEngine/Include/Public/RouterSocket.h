#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API RouterSocket : public SocketBase
	{
	public:
		RouterSocket(Context *Context, int BufferSize);
	};
}

#endif