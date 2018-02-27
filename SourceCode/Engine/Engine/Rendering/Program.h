// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class Program : public NativeType
		{
		public:
			Program(IDevice *Device, Handle Handle) :
				NativeType(Device, Handle)
			{
			}
		};
	}
}

#endif