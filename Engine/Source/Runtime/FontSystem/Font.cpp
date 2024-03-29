// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\Texture.h>

namespace Engine
{
	using namespace RenderSystem;

	namespace FontSystem
	{
		using namespace Private;

		Font::Font(void) :
			m_RenderType(FontRenderTypes::Mesh),
			m_Texture(nullptr),
			m_Characters(FontSystemAllocators::FontSystemAllocator)
		{
		}

		Font::~Font(void)
		{
			DeviceInterface* device = RenderManager::GetInstance()->GetDevice();

			if (m_Texture != nullptr)
				device->DestroyTexture(m_Texture);

			if (m_RenderType == FontRenderTypes::Mesh)
			{
				for (auto& character : m_Characters)
					device->DestroyMesh(character.GetSecond().GetMesh());
			}
		}

		void Font::SetName(const String& Name)
		{
			SetName(Name.ChangeType<char16>());
		}

		void Font::SetName(const WString& Name)
		{
			if (m_RenderType == FontRenderTypes::Texture)
				m_Texture->SetName(Name);
		}

		Font::Character* Font::GetCharacter(const uint64& CharCode)
		{
			if (m_Characters.Contains(CharCode))
				return &m_Characters[CharCode];

			return nullptr;
		}
	}
}