#include "Include\Public\PairSocket.h"

PairSocket::PairSocket(Context *Context, int BufferSize) :
	SocketBase(Context, BufferSize, SocketType::Pair)
{
}