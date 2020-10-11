// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_TYPE_H
#define NATIVE_TYPE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			class DeviceThread;

			namespace Commands
			{
				class DrawCommand;
			}
		}

		using namespace Private;
		using namespace Private::Commands;

		class NativeType
		{
			friend class DrawCommand;
			friend class Shader;
			friend class ShaderConstantSupplier;
			friend class DeviceInterface;

		public:
			typedef uint32 Handle;

		public:
			NativeType(DeviceThread* Device, Handle Handle) :
				m_Device(Device),
				m_Handle(Handle)
			{
			}

		protected:
			INLINE DeviceThread* GetDevice(void) const
			{
				return m_Device;
			}

			INLINE Handle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			DeviceThread* m_Device;
			Handle m_Handle;
		};
	}
}

#endif