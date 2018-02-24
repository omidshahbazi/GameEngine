// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class IDevice;

		class Texture
		{
		public:
			typedef uint32 Handle;

		public:
			Texture(IDevice *Device, Handle Handle);

			~Texture(void);

		private:
			IDevice *m_Device;
			Handle m_Handle;
		};
	}
}

#endif