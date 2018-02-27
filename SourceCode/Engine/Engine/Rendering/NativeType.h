// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_TYPE_H
#define NATIVE_TYPE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class IDevice;

		class NativeType
		{
		public:
			typedef uint32 Handle;

		public:
			NativeType(IDevice *Device, Handle Handle) :
				m_Device(Device),
				m_Handle(Handle)
			{
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