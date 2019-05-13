// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	namespace FontSystem
	{
		using namespace Private;

		Font::Font(void) :
			m_Meshes(&FontSystemAllocators::FontSystemAllocator)
		{
		}

		Mesh * Font::GetMesh(const uint64 & CharCode)
		{
			DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

			if (m_Meshes.Contains(CharCode))
				return m_Meshes[CharCode];

			if (m_MesheInfos.Contains(CharCode))
			{
				MeshInfo *info = m_MesheInfos[CharCode];

				Mesh *mesh = device->CreateMesh(info, IDevice::BufferUsages::StaticDraw);

				m_Meshes.Add(CharCode, mesh);

				return mesh;
			}

			return nullptr;
		}
	}
}