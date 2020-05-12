// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>

namespace Engine
{
	namespace FontSystem
	{
		using namespace Private;

		Font::Font(void) :
			m_RenderType(RenderTypes::Mesh),
			m_Characters(&FontSystemAllocators::FontSystemAllocator)
		{
		}

		Font::Character* Font::GetCharacter(const uint64& CharCode)
		{
			if (m_Characters.Contains(CharCode))
				return &m_Characters[CharCode];

			return nullptr;
		}
	}
}