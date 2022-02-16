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
			GPUBuffer(ThreadedDevice* Device, ResourceHandle Handle, uint32 Size);
			virtual	~GPUBuffer(void);

			virtual void SetNameInternal(const WString& Name) override;

		public:
			virtual void Reset(void)
			{
				Seek(0);
			}

		protected:
			byte* GetBuffer(GPUBufferAccess Access, bool Directly = false);
			void UngetBuffer(bool Directly = false);

			byte* Lock(GPUBufferAccess Access, bool Directly = false);
			void Unlock(bool Directly = false);

			void CopyTo(ResourceHandle Handle);

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

			byte* Seek(uint32 Index)
			{
				return (m_CurrentBuffer = m_StartBuffer + Index);
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

			bool m_IsLocked;
			byte* m_StartBuffer;
			byte* m_CurrentBuffer;

			GPUBufferAccess m_LastLockAccess;
		};
	}
}

#endif