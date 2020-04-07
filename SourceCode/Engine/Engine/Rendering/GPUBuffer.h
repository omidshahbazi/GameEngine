// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <Rendering\NativeType.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API GPUBuffer : public NativeType
		{
		protected:
			GPUBuffer(IDevice* Device, Handle Handle, uint32 Size, IDevice::BufferTypes Type, IDevice::BufferAccess Access);

		public:
			void Reset(void)
			{
				m_CurrentBuffer = m_StartBuffer;
			}

		protected:
			byte* Lock(void);
			void Unlock(void);

			uint32 GetSize(void) const
			{
				return m_Size;
			}

			byte* GetStartBuffer(void) const
			{
				return m_StartBuffer;
			}

			byte* GetCurrentBuffer(void) const
			{
				return m_CurrentBuffer;
			}

			byte* GetEndBuffer(void) const
			{
				return (m_StartBuffer + m_Size);
			}

			byte* Move(uint32 Count)
			{
				return (m_CurrentBuffer += Count);
			}
	
		public:
			bool GetIsLocked(void) const
			{
				return m_IsLocked;
			}

		private:
			uint32 m_Size;
			IDevice::BufferTypes m_Type;
			IDevice::BufferAccess m_Access;

			bool m_IsLocked;
			byte* m_StartBuffer;
			byte* m_CurrentBuffer;
		};
	}
}

#endif