#ifndef SOCKETBASE_H
#define SOCKETBASE_H

#include "Common.h"

namespace NetworkEngine
{
	class Context;
	class Buffer;

	enum class SocketType
	{
		Pair
	};

	class NETWORK_ENGINE_API SocketBase
	{
	public:
		SocketBase(Context *Context, int BufferSize, SocketType Type);
		~SocketBase(void);

		virtual void Bind(const char *Address);

		virtual Buffer *Read(void) const;
		virtual void Write(const Buffer * const Buffer) const;

	private:
		void *m_Socket;
		int m_BufferSize;
	};
}

#endif