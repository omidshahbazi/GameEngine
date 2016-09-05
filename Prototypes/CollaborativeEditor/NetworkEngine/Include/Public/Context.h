#ifndef CONTEXT_H
#define CONTEXT_H

#include "Common.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API Context
	{
		friend class SocketBase;

	public:
		Context(void);

		~Context(void);

	private:
		void *m_Context;
	};
}

#endif