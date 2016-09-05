#include "Include\Public\Buffer.h"
#include <memory>

namespace NetworkEngine
{
	Buffer::Buffer(const void *Buffer, int Lenght) :
		m_Buffer(NULL),
		m_Length(0)
	{
		m_Buffer = malloc(Lenght);
		memcpy(m_Buffer, Buffer, m_Length);
	}

	Buffer::~Buffer(void)
	{
		free(m_Buffer);
	}
}