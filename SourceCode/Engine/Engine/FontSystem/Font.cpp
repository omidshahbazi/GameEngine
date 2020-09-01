// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Rendering;

	namespace FontSystem
	{
		using namespace Private;

		Font::Font(void) :
			m_RenderType(RenderTypes::Mesh),
			m_Texture(nullptr),
			m_Characters(FontSystemAllocators::FontSystemAllocator)
		{
		}

		Font::~Font(void)
		{
			DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

			if (m_Texture != nullptr)
				device->DestroyTexture(m_Texture);

			if (m_RenderType == RenderTypes::Mesh)
			{
				for each (auto & character in m_Characters)
					device->DestroyMesh(character.GetSecond().GetMesh());
			}
		}

		Font::Character* Font::GetCharacter(const uint64& CharCode)
		{
			if (m_Characters.Contains(CharCode))
				return &m_Characters[CharCode];

			return nullptr;
		}
	}
}