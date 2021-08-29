// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			class GPUConstantBuffer;
		}

		using namespace Private;

		class RENDERING_API ConstantBuffer
		{
			friend class GPUConstantBuffer;

		public:
			typedef GPUBuffer::Handle Handle;

		public:
			ConstantBuffer(uint32 Size);
			~ConstantBuffer(void);

			void Reset(void);

			void Move(uint32 Offset);

			void Copy(const ConstantBuffer* const Other);

			void Set(const byte* Data, uint16 Size);

			template<typename T, int Size = sizeof(T)>
			void Set(const T* Data)
			{
				if (Size > GetSize())
					return;

				Set(ReinterpretCast(const byte*, Data), Size);
			}

			template<typename T, int Size = sizeof(T)>
			T* Get(void)
			{
				if (Size > GetSize())
					return nullptr;

				return ReinterpretCast(T*, m_CurrentCachedData);
			}

			template<typename T, int Size = sizeof(T)>
			const T* Get(void) const
			{
				if (Size > GetSize())
					return nullptr;

				return ReinterpretCast(T*, m_CurrentCachedData);
			}

			uint32 GetSize(void) const
			{
				return m_Size;
			}

		private:
			uint32 m_Size;
			byte* m_CachedData;
			byte* m_CurrentCachedData;
		};
	}
}

#endif