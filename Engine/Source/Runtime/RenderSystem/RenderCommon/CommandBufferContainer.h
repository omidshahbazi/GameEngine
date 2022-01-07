// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMAND_BUFFER_CONTAINER_H
#define COMMAND_BUFFER_CONTAINER_H

#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderCommon
	{
		class CommandBufferContainer : private Vector<byte>
		{
		public:
			CommandBufferContainer(AllocatorBase* Allocator) :
				Vector<byte>(Allocator),
				m_ReadIndex(0)
			{
			}

			template<typename T>
			INLINE void Append(const T& Item)
			{
				const uint16 size = sizeof(T);

				const uint32 startIndex = Extend(size);

				PlatformMemory::Copy(ReinterpretCast(const byte*, &Item), GetData() + startIndex, size);
			}

			INLINE void Clear(void)
			{
				Vector<byte>::Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return Vector<byte>::GetSize();
			}

			template<typename T>
			INLINE bool Read(T& Item)
			{
				if (m_ReadIndex >= Vector<byte>::GetSize())
					return false;

				const uint16 size = sizeof(T);

				PlatformMemory::Copy(Vector<byte>::GetData() + m_ReadIndex, ReinterpretCast(byte*, &Item), size);

				m_ReadIndex += size;

				return true;
			}

			INLINE void ResetRead(void)
			{
				m_ReadIndex = 0;
			}

		private:
			uint32 m_ReadIndex;
		};
	}
}

#endif