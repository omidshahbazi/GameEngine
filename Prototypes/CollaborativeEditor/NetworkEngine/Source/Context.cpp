#include "Include\Public\Context.h"
#include <zmq.h>

Context::Context(void) :
	m_Context(NULL)
{
	m_Context = zmq_ctx_new();
}

Context::~Context(void)
{
	zmq_ctx_destroy(m_Context);
}