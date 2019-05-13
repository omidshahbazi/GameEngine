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
			m_InitialCharacters(&FontSystemAllocators::FontSystemAllocator),
			m_ReadyCharacter(&FontSystemAllocators::FontSystemAllocator)
		{
		}

		Font::Character * Font::GetMesh(const uint64 & CharCode)
		{
			if (m_ReadyCharacter.Contains(CharCode))
				return m_ReadyCharacter[CharCode];

			if (m_InitialCharacters.Contains(CharCode))
			{
				DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				auto &ch = m_InitialCharacters[CharCode];

				Mesh *mesh = device->CreateMesh(ch.GetMeshInfo(), IDevice::BufferUsages::StaticDraw);

				ch.SetMesh(mesh);

				m_ReadyCharacter.Add(CharCode, &ch);

				return &ch;
			}

			return nullptr;
		}
	}
}