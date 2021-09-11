// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <RenderSystem\NativeType.h>
#include <RenderCommon\Enums.h>

namespace Engine
{
	using namespace RenderCommon;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API GPUBuffer : public NativeType
		{
		protected:
			GPUBuffer(ThreadedDevice* Device, ResourceHandle Handle, uint32 Size, GPUBufferTypes Type);
			virtual	~GPUBuffer(void);

		public:
			virtual void Reset(void)
			{
				m_CurrentBuffer = m_StartBuffer;
			}

		protected:
			byte* GetBuffer(GPUBufferAccess Access, bool Directly = false);
			void UngetBuffer(bool Directly = false);

			byte* Lock(GPUBufferAccess Access, bool Directly = false);
			void Unlock(bool Directly = false);

			uint32 GetSize(void) const
			{
				return m_Size;
			}

			GPUBufferTypes GetType(void) const
			{
				return m_Type;
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

			GPUBufferAccess GetLastLockAccess(void) const
			{
				return m_LastLockAccess;
			}

		public:
			bool GetIsLocked(void) const
			{
				return m_IsLocked;
			}

		private:
			uint32 m_Size;
			GPUBufferTypes m_Type;

			bool m_IsLocked;
			byte* m_StartBuffer;
			byte* m_CurrentBuffer;

			GPUBufferAccess m_LastLockAccess;
		};
	}
}

#endif