// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <FontSystem\Font.h>

namespace Engine
{
	using namespace Containers;

	namespace FontSystem
	{
		using namespace Private;

		//template<typename BaseType>
		//BaseType *Allocate(void)
		//{
		//	return ReinterpretCast(BaseType*, AllocateMemory(&FontSystemAllocators::FontSystemAllocator, sizeof(BaseType)));
		//}

		//template<typename BaseType>
		//void Deallocate(BaseType *Ptr)
		//{
		//	DeallocateMemory(&FontSystemAllocators::FontSystemAllocator, Ptr);
		//}

		SINGLETON_DEFINITION(FontManager)

		FontManager::FontManager(void)
		{
		}

		FontManager::~FontManager(void)
		{
		}

		Font *FontManager::LoadFont(const ByteBuffer &Data)
		{
			Font *font = nullptr;1

			return font;
		}
	}
}