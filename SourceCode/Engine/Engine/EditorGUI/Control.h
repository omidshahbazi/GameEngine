// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_H
#define CONTROL_H

namespace Engine
{
	namespace Rendering
	{
		class DeviceInterface;
	}

	using namespace Rendering;

	namespace EditorGUI
	{
		class EDITORGUI_API Control
		{
		public:
			virtual void Render(DeviceInterface *Device) const = 0;
		};
	}
}

#endif