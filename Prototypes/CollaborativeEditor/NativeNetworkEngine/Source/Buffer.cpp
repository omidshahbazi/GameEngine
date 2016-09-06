#include "Include\Public\Buffer.h"
#include <memory>

namespace NetworkEngine
{
	Buffer::Buffer(const void *Buffer, int Lenght) :
		m_Buffer(malloc(Lenght)),
		m_Length(Lenght)
	{
		memcpy(m_Buffer, Buffer, m_Length);
	}

	Buffer::~Buffer(void)
	{
		free(m_Buffer);
	}
}