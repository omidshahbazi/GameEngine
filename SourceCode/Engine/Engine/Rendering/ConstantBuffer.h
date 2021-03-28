// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class Progrm;

		class RENDERING_API ConstantBuffer : public GPUBuffer
		{
			friend class Program;

		protected:
			ConstantBuffer(Program* Program, uint32 Size, Handle Handle);

		public:
			virtual void Lock(Access Access = Access::ReadOnly);
			virtual void Unlock(void);

			void Move(uint32 Offset);

			void Set(const byte* Data, uint16 Size);

			template<typename T>
			T* Get(void)
			{
				return ReinterpretCast(T*, GetCurrentBuffer());
			}

			template<typename T>
			const T* Get(void) const
			{
				return ReinterpretCast(T*, GetCurrentBuffer());
			}
		};
	}
}

#endif