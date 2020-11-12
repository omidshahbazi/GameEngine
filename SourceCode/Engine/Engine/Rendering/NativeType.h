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
			class ThreadedDevice;

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
#if X64
			typedef uint64 Handle;
#else
			typedef uint32 Handle;
#endif

		public:
			NativeType(ThreadedDevice* Device, Handle Handle) :
				m_Device(Device),
				m_Handle(Handle)
			{
			}

		protected:
			INLINE ThreadedDevice* GetDevice(void) const
			{
				return m_Device;
			}

			INLINE Handle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			ThreadedDevice* m_Device;
			Handle m_Handle;
		};
	}
}

#endif