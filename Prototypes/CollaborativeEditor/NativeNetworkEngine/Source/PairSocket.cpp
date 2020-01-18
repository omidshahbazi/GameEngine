#include "Include\Public\PairSocket.h"

namespace NetworkEngine
{
	PairSocket::PairSocket(Context *Context, int BufferSize) :
		SocketBase(Context, BufferSize, SocketType::Pair)
	{
	}
}