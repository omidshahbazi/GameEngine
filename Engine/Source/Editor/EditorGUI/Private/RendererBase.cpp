// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\RendererBase.h>

namespace Engine
{
	namespace EditorGUI
	{
		namespace Private
		{
			RendererBase::RendererBase(void) :
				m_Rotation(0)
			{
			}

			void RendererBase::SetMaterial(Material* Material)
			{
				m_Material = *Material;
			}
		}
	}
}