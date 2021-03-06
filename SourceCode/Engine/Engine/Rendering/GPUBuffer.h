// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <Rendering\NativeType.h>

namespace Engine
{
	namespace Rendering
	{
		class IDevice;

		class RENDERING_API GPUBuffer : public NativeType
		{
		public:
			enum class Types
			{
				Constant = 0,
				Vertex,
				Index,
				Pixel
			};

			enum class Access
			{
				ReadOnly = 0,
				WriteOnly,
				ReadAndWrite
			};

		protected:
			GPUBuffer(ThreadedDevice* Device, Handle Handle, uint32 Size, Types Type);
			virtual	~GPUBuffer(void);

		public:
			virtual void Reset(void)
			{
				m_CurrentBuffer = m_StartBuffer;
			}

		protected:
			byte* GetBuffer(Access Access, bool Directly = false);
			void UngetBuffer(bool Directly = false);

			byte* Lock(Access Access, bool Directly = false);
			void Unlock(bool Directly = false);

			uint32 GetSize(void) const
			{
				return m_Size;
			}

			Types GetType(void) const
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

			Access GetLastLockAccess(void) const
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
			Types m_Type;

			bool m_IsLocked;
			byte* m_StartBuffer;
			byte* m_CurrentBuffer;

			Access m_LastLockAccess;
		};
	}
}

#endif