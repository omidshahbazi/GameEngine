#ifndef SOCKETBASE_H
#define SOCKETBASE_H

#include "Common.h"

namespace NetworkEngine
{
	class Context;
	class Buffer;

	enum class SocketType
	{
		Pair,
		Publisher,
		Subscriber,
		Request,
		Reply,
		Dealer,
		Router,
		Pull,
		Push,
		//XPUB
		//XSUB
		Stream
	};

	class NETWORK_ENGINE_API SocketBase
	{
	public:
		SocketBase(Context *Context, SocketType Type);
		~SocketBase(void);

		virtual void Bind(const byte *Address);
		virtual void Connect(const byte *Address);

		virtual uint16 Read(const byte *Buffer, uint16 Count) const;
		virtual void Read(const Buffer * Buffer) const;

		virtual void Write(const Buffer * const Buffer) const;

	private:
		void *m_Socket;
	};
}

#endif