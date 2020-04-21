// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERABLE_WINDOW_H
#define RENDERABLE_WINDOW_H

#include <EditorGUI\Control.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API RenderableWindow : public Control
		{
		public:
			RenderableWindow(void);

			virtual void Render(EditorRenderDeviceBase* Device) const override;

		private:
			Mesh* m_QuadMesh;
			Material* m_BackgroundMaterial;
			Material* m_TitleBarMaterial;
		};
	}
}

#endif