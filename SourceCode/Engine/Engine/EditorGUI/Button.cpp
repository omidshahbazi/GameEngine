// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void)
		{
		}

		void Button::Render(EditorRenderDeviceBase* Device) const
		{
			m_Sprite.Render(Device);
		}

		void Button::OnSizeChanged(void)
		{
			m_Sprite.SetElementDimension(GetRect().Size);
		}
	}
}