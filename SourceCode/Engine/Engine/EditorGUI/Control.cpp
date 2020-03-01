// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Control.h>

namespace Engine
{
	namespace EditorGUI
	{
		Control::Control(Control* Parent) :
			m_Parent(Parent),
			m_Rect(0, 0, 1, 1)
		{
		}

		void Control::SetRect(const RectI& Value)
		{
			m_Rect = Value;

			CALL_CALLBACK(IListener, OnRectChanged, this, Value);
		}
	}
}