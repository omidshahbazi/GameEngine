// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>

namespace Engine
{
	namespace Rendering
	{
		Program::Program(IDevice *Device, Handle Handle) :
			m_Device(Device),
			m_Handle(Handle)
		{
		}

		Program::~Program(void)
		{
		}
	}
}