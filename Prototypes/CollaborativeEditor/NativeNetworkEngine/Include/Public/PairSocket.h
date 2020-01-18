#ifndef PAIRSOCKET_H
#define PAIRSOCKET_H

#include "SocketBase.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API PairSocket : public SocketBase
	{
	public:
		PairSocket(Context *Context, int BufferSize);
	};
}

#endif