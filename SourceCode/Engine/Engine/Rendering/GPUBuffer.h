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
		class GPUBuffer : public NativeType
		{
		protected:
			GPUBuffer(IDevice* Device, Handle Handle, IDevice::BufferTypes Type, IDevice::BufferAccess Access);

		public:
			void* Map(void);
			void Unmap(void);

		private:
			IDevice::BufferTypes m_Type;
			IDevice::BufferAccess m_Access;
		};
	}
}

#endif