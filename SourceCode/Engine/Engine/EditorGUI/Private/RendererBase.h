// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

namespace Engine
{
	namespace EditorGUI
	{
		class EditorRenderDeviceBase;

		namespace Private
		{
			class RendererBase
			{
			public:
				RendererBase(void);
				virtual ~RendererBase(void)
				{
				}

				virtual void Render(EditorRenderDeviceBase* Device) const = 0;
			};
		}
	}
}

#endif