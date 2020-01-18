#ifndef BUFFER_H
#define BUFFER_H

#include "Common.h"

namespace NetworkEngine
{
	class NETWORK_ENGINE_API Buffer
	{
	public:
		Buffer(uint16 Count);
		Buffer(const void *Buffer, uint16 Size);
		~Buffer(void);

		INLINE const void *GetBuffer(void) const
		{
			return m_Buffer;
		}

		INLINE uint16 GetCount(void) const
		{
			return m_Count;
		}

		INLINE uint16 GetSize(void) const
		{
			return m_Size;
		}

	private:
		void *m_Buffer;
		uint16 m_Count;
		uint16 m_Size;
	};
}

#endif