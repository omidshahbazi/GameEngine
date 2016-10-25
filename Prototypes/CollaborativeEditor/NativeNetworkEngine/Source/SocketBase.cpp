#include "Include\Public\SocketBase.h"
#include "Include\Public\Context.h"
#include "Include\Public\Buffer.h"
#include <zmq.h>

namespace NetworkEngine
{
	int SocketTypeToZMQSocketType(SocketType Type)
	{
		switch (Type)
		{
		case SocketType::Pair:
			return ZMQ_PAIR;
		case SocketType::Publisher:
			return ZMQ_PUB;
		case SocketType::Request:
			return ZMQ_REQ;
		case SocketType::Reply:
			return ZMQ_REP;
		case SocketType::Dealer:
			return ZMQ_DEALER;
		case SocketType::Router:
			return ZMQ_ROUTER;
		case SocketType::Pull:
			return ZMQ_PULL;
		case SocketType::Push:
			return ZMQ_PUSH;
		case SocketType::Stream:
			return ZMQ_STREAM;
		}

		return 0;
	}

	SocketBase::SocketBase(Context *Context, SocketType Type) :
		m_Socket(NULL)
	{
		m_Socket = zmq_socket(Context->m_Context, SocketTypeToZMQSocketType(Type));
	}

	SocketBase::~SocketBase(void)
	{
		zmq_close(m_Socket);
	}

	void SocketBase::Bind(const char * Address)
	{
		zmq_bind(m_Socket, Address);
	}

	void SocketBase::Connect(const char * Address)
	{
		zmq_connect(m_Socket, Address);
	}

	Buffer *SocketBase::Read(void) const
	{
		static void *buffer = malloc(m_BufferSize);

		int length = zmq_recv(m_Socket, buffer, m_BufferSize, 0);

		if (length == 0)
			return NULL;

		return new Buffer(buffer, length);
	}

	void SocketBase::Write(const Buffer * const Buffer)const
	{
		int result = zmq_send(m_Socket, Buffer->GetBuffer(), Buffer->GetLength(), 0);
	}
}