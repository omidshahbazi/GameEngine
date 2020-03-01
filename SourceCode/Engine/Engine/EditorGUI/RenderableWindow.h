// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERABLE_WINDOW_H
#define RENDERABLE_WINDOW_H

#include <EditorGUI\Control.h>
#include <Rendering\Material.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API RenderableWindow : public Control, Control::IListener
		{
		public:
			RenderableWindow(void);

			virtual void Render(DeviceInterface* Device) const override;

		protected:
			virtual void OnRectChanged(Control* Control, const RectI& Rect) override;

		private:
			Mesh* m_QuadMesh;
			Material* m_TitleBarMaterial;

			Matrix4F m_ProjMat;
		};
	}
}

#endif