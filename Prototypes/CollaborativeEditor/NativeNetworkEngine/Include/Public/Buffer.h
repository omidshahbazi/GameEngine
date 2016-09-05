#ifndef BUFFER_H
#define BUFFER_H

#include "Common.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API Buffer
	{
	public:
		Buffer(const void *Buffer, int Lenght);
		~Buffer(void);

		const void *GetBuffer(void) const
		{
			return m_Buffer;
		}

		int GetLength(void) const
		{
			return m_Length;
		}

	private:
		void *m_Buffer;
		int m_Length;
	};
}

#endif