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

			namespace Commands
			{
				class DrawCommand;
			}
		}

		using namespace Private;
		using namespace Private::Commands;

		class RENDERING_API ConstantBuffer : public GPUBuffer
		{
			friend class DeviceInterface;
			friend class ProgramConstantHolder;
			friend class ProgramConstantSupplier;
			friend class BuiltiInProgramConstants;
			friend class DrawCommand;

		protected:
			ConstantBuffer(ThreadedDevice* Device, uint32 Size, Handle Handle);

		public:
			~ConstantBuffer(void);

			void Reset(void) override;

			void Move(uint32 Offset);

			void Copy(const ConstantBuffer& Other);

			void Set(const byte* Data, uint16 Size);

			template<typename T, int Size = sizeof(T)>
			void Set(const T* Data)
			{
				if (Size > GetSize())
					return;

				Set(ReinterpretCast(const byte*, Data), Size);
			}

			template<typename T, int Size = sizeof(T)>
			T* Get(Access Access = Access::ReadOnly)
			{
				if (Size > GetSize())
					return nullptr;

				if (Access != Access::ReadOnly)
					m_IsDirty = true;

				return ReinterpretCast(T*, m_CurrentCachedData);
			}

			template<typename T, int Size = sizeof(T)>
			const T* Get(void) const
			{
				if (Size > GetSize())
					return nullptr;

				return ReinterpretCast(T*, m_CurrentCachedData);
			}

		private:
			void UploadToGPU(void);

			bool GetIsDirty(void) const
			{
				return m_IsDirty;
			}

		private:
			byte* m_CachedData;
			byte* m_CurrentCachedData;
			bool m_IsDirty;
		};
	}
}

#endif