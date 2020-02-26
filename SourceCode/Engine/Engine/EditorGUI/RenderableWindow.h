// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERABLE_WINDOW_H
#define RENDERABLE_WINDOW_H

#include <EditorGUI\Control.h>

namespace Engine
{
	namespace EditorGUI
	{
		class EDITORGUI_API RenderableWindow : public Control
		{
		public:
			virtual void Render(DeviceInterface *Device) const override;

		};
	}
}

#endif