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
			typedef uint32 Handle;

		public:
			Window(IDevice *Device, Handle Handle);

			~Window(void);

		private:
			IDevice *m_Device;
			Handle m_Handle;
		};
	}
}

#endif