// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class IDevice;

		class Window
		{
		public:
#if X64
			typedef uint64 Handle;
#else
			typedef uint32 Handle;
#endif

		public:
			Window(IDevice *Device, Handle Handle) :
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