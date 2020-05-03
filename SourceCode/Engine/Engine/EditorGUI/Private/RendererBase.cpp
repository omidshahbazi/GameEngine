// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\RendererBase.h>
#include <EditorGUI\EditorRenderDeviceBase.h>

namespace Engine
{
	namespace EditorGUI
	{
		namespace Private
		{
			RendererBase::RendererBase(void) :
				m_Pass(nullptr),
				m_Rotation(0)
			{
			}

			void RendererBase::SetMaterial(const Material& Material)
			{
				m_Material = Material;
				m_Pass = &m_Material.GetPasses()[0];
			}
		}
	}
}