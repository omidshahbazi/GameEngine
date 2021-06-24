// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class DeviceInterface;
		class ProgramConstantSupplier;

		namespace Private
		{
			class ProgramConstantHolder;
			class BuiltiInProgramConstants;
		}

		using namespace Private;

		class RENDERING_API ConstantBuffer : public GPUBuffer
		{
			friend class DeviceInterface;
			friend class ProgramConstantHolder;
			friend class ProgramConstantSupplier;
			friend class BuiltiInProgramConstants;

		protected:
			ConstantBuffer(ThreadedDevice* Device, uint32 Size, Handle Handle);

		public:
			virtual void Lock(Access Access = Access::ReadOnly);
			virtual void Unlock(void);

			void Move(uint32 Offset);

			void Set(ConstantBuffer& Other);
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

				return ReinterpretCast(T*, GetCurrentBuffer());
			}

			template<typename T, int Size = sizeof(T)>
			const T* Get(void) const
			{
				if (Size > GetSize())
					return nullptr;

				return ReinterpretCast(T*, GetCurrentBuffer());
			}

		private:
			void LockDirectly(Access Access = Access::ReadOnly);
			void UnlockDirectly(void);

		private:
			ConstantBuffer* Clone(void) const;
		};
	}
}

#endif