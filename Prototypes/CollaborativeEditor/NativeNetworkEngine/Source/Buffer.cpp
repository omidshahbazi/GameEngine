#include "Include\Public\Buffer.h"
#include <memory>

namespace NetworkEngine
{
	Buffer::Buffer(uint16 Count) :
		m_Buffer(malloc(Count)),
		m_Count(Count),
		m_Size(0)
	{
		memset(m_Buffer, 0, m_Count);
	}

	Buffer::Buffer(const void *Buffer, uint16 Size) :
		m_Buffer(malloc(Size)),
		m_Count(Size),
		m_Size(Size)
	{
		memcpy(m_Buffer, Buffer, m_Size);
	}

	Buffer::~Buffer(void)
	{
		free(m_Buffer);
	}
}