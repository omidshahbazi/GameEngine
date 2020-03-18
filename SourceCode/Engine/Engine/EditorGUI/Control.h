// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <Containers\ListenerContainer.h>
#include <EditorGUI\EditorRenderDeviceBase.h>

namespace Engine
{
	namespace Rendering
	{
		class DeviceInterface;
	}

	using namespace Rendering;
	using namespace Containers;
	using namespace MathContainers;

	namespace EditorGUI
	{
		class EDITORGUI_API Control
		{
		public:
			class IListener
			{
			public:
				virtual void OnRectChanged(Control* Control, const RectI& Rect) = 0;
			};

			LISTENER_DECLARATION(IListener)

		public:
			Control(Control* Parent = nullptr);

			virtual void Render(EditorRenderDeviceBase* Device) const = 0;

			const RectI& GetRect(void) const
			{
				return m_Rect;
			}

			void SetRect(const RectI& Value);

			const RectI& GetClienctRect(void) const
			{
				return m_ClientRect;
			}

			Control* GetParent(void)
			{
				return m_Parent;
			}

		private:
			Control* m_Parent;
			RectI m_Rect;
			RectI m_ClientRect;
		};
	}
}

#endif