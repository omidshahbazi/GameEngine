// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>

namespace Engine
{
	namespace FontSystem
	{
		using namespace Private;

		Font::Font(void) :
			m_Meshes(&FontSystemAllocators::FontSystemAllocator)
		{
		}
	}
}