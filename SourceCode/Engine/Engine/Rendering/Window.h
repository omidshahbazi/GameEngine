// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Rendering\NativeType.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class Window : public NativeType
		{
		public:
			Window(IDevice *Device, Handle Handle) :
				NativeType(Device, Handle)
			{
			}
		};
	}
}

#endif