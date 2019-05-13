// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <FontSystem\Font.h>
#include <Utility\AssetParser\InternalModelParser.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility::AssetParser;
	using namespace Rendering;

	namespace FontSystem
	{
		using namespace Private;

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&FontSystemAllocators::FontSystemAllocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&FontSystemAllocators::FontSystemAllocator, Ptr);
		}

		SINGLETON_DEFINITION(FontManager)

			FontManager::FontManager(void)
		{
		}

		FontManager::~FontManager(void)
		{
		}

		Font *FontManager::LoadFont(const ByteBuffer &Data)
		{
			Font *font = Allocate<Font>();
			Construct(font);

			auto &mesheInfos = font->m_MesheInfos;

			uint64 index = 0;
			while (index < Data.GetSize())
			{
				uint64 charCode = Data.ReadValue<uint64>(index);
				index += sizeof(uint64);

				uint64 meshDataSize = Data.ReadValue<uint64>(index);
				index += sizeof(uint64);

				const byte* meshData = Data.ReadValue(index, meshDataSize);
				index += meshDataSize;

				MeshInfo *meshInfo = Allocate<MeshInfo>();
				Construct(meshInfo, &FontSystemAllocators::FontSystemAllocator);

				InternalModelParser parser;
				parser.Parse(ByteBuffer(ConstCast(byte*, meshData), meshDataSize), *meshInfo);

				mesheInfos.Add(charCode, meshInfo);
			}

			return font;
		}

		void FontManager::DestroyFont(Font * Font)
		{
		}
	}
}