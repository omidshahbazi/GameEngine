// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_TYPE_H
#define NATIVE_TYPE_H

#include <Common\PrimitiveTypes.h>
#include <ResourceSystem\ResourceHandle.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}
		}

		using namespace Private::Commands;
		class IDevice;

		class NativeType
		{
			friend class DeviceInterface;
			friend class DrawCommand;
			friend class ProgramConstantSupplier;

		public:
			typedef uint32 Handle;

		public:
			NativeType(IDevice *Device, Handle Handle) :
				m_Device(Device),
				m_Handle(Handle)
			{
			}

		protected:
			INLINE IDevice *GetDevice(void) const
			{
				return m_Device;
			}

			INLINE Handle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			IDevice *m_Device;
			Handle m_Handle;
		};
	}
}

#endif