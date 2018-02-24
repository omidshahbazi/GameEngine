// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class IDevice;

		class Program
		{
		public:
			typedef uint32 Handle;

		public:
			Program(IDevice *Device, Handle Handle);

			~Program(void);

		private:
			IDevice *m_Device;
			Handle m_Handle;
		};
	}
}

#endif