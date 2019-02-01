// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <Rendering\NativeType.h>

namespace Engine
{
	namespace Rendering
	{
		class GPUBuffer : public NativeType
		{
			friend class DeviceInterface;

		private:
			GPUBuffer(IDevice *Device, Handle Handle, uint32 Count) :
				NativeType(Device, Handle),
				m_Count(Count)
			{
			}

		public:
			uint32 GetCount(void) const
			{
				return m_Count;
			}

		private:
			uint32 m_Count;
		};
	}
}

#endif